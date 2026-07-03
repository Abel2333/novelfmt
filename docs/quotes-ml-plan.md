# Chinese Quote Normalization: ML Plan

This document describes a future machine learning approach for Chinese quote normalization. It is intended as a replacement path for the current rule-based implementation.

It is not a description of the current code. The current formatter still uses paragraph-local rules; this document is for a later standalone experiment and deployment track.

## 1. Problem Definition

### 1.1 Goal

Normalize mixed-source quote marks in novel text into Chinese corner quotes:

- Primary quotes: `「」`
- Secondary quotes: `『』`

Target cases include:

- Complete source quotes: `"He said I am here"` -> `「He said I am here」`
- Missing opening quote: `He said I am here"` -> `「He said I am here」`
- Missing closing quote: `"He said I am here` -> `「He said I am here」`
- No source quotes, but semantically direct speech: `He said I am here` -> `「He said I am here」`
- Nested quotes: `"He said 'hello'"` -> `「He said『hello』」`
- Multi-paragraph dialogue: speech opened in one paragraph and continued in the next

### 1.2 Limitations of the Current Rule-Based Approach

The current rules mainly rely on a paragraph-local state machine plus a small number of heuristics. The main limitations are:

- Cross-paragraph dialogue continuation is fragile
- Recovery is weak when the original text has no quote marks at all
- Fault tolerance is limited for badly damaged source text
- Maintainability degrades quickly as more special cases are added

## 2. Modeling Principles

### 2.1 Core Idea

The model should not directly "repair source quote characters". It should predict the **semantic span boundaries** of quoted content.

The pipeline is:

1. Filter all source quote characters from the input
2. Run sequence labeling on the quote-filtered character stream
3. Reinsert `「」` and `『』` solely from predicted span boundaries

Advantages:

- Input remains stable even when source quote direction is wrong
- Missing opening quotes, missing closing quotes, and mixed half-width / curly quotes become easier to normalize
- The training target no longer depends on noisy source quote forms

### 2.2 An Important Limitation

"No-quote recovery" is the hardest part of the task.

For example:

```text
He said I am here
```

This may be:

- plain narration
- free indirect discourse
- direct speech with all source quotes missing

So the document keeps the long-term goal of recovering quotes from pure semantics, but implementation should proceed in stages:

1. First cover samples that still contain damaged quote evidence
2. Then gradually expand to samples with no source quotes at all

Otherwise the model may overfit into adding quotes whenever it sees speech-like wording.

## 3. Label Design

### 3.1 Two-Level BIOES Tag Set

Use 9 labels:

```text
O               outside any quote
B1 I1 E1 S1     primary quoted span (rendered as 「」)
B2 I2 E2 S2     secondary quoted span (rendered as 『』)
```

### 3.2 Semantics of Two-Level Nesting

This is not a general nested NER setup. It is a **linear tag scheme specialized for two quote levels**.

Example:

```text
Normalized text: 「He said『hello』ah」
Content chars:   He said hello ah
Labels:          B1 I1 B2 E2 E1
```

Meaning:

- `B1 ... E1` means the token stream is inside a primary quote
- `B2 ... E2` inside that region means a secondary quote nested within the primary span

This design is suitable only when:

- nesting depth is at most two
- quote levels are fixed to `「」` and `『』`

If arbitrary-depth nesting is ever required, the modeling approach should be changed.

### 3.3 CRF Transition Constraints

The CRF should strongly constrain illegal transitions to avoid impossible label sequences.

At minimum:

- `O` must not jump directly to `I1 / E1 / I2 / E2`
- `B1` should only transition to `I1 / E1 / B2 / S2`
- `B2` should only transition to `I2 / E2`
- `E2` should only return to the primary span or end it

Besides the transition matrix itself, it is also worth initializing:

- start transitions
- end transitions

Otherwise sentence-initial and sentence-final illegal labels may still appear.

## 4. Model Architecture

### 4.1 Recommended Baseline

Start with:

- `hfl/chinese-roberta-wwm-ext`
- a linear classifier head
- a CRF layer

That is:

```text
BERT/RoBERTa -> Linear -> CRF
```

This is the most practical first-stage setup because:

- Chinese pretrained models are mature
- the implementation path is straightforward
- ONNX export and C++ inference remain manageable

### 4.2 Whether to Add BiLSTM

`BERT + BiLSTM + CRF` can be evaluated later, but it should not be the starting point.

Suggested order:

1. Build `BERT + CRF`
2. Add `BiLSTM` only if the baseline plateaus after the dataset grows

Starting with too many moving parts makes it harder to isolate failure modes.

## 5. Input Representation and Alignment

### 5.1 Do Not Assume "One Chinese Character = One Token"

This is the most important correction to the original draft.

Although many Chinese characters map cleanly under BERT tokenization, it is not guaranteed:

- punctuation and special symbols can behave differently
- ASCII, digits, and mixed Latin text may split into multiple subwords
- uncommon characters may become `[UNK]`

Training therefore needs **explicit alignment from character-level labels to subword-level input**.

Recommended approach:

1. Build samples and BIOES labels at character level
2. Preserve the character-to-token mapping during tokenization
3. Assign supervision only to the first token of each character
4. Ignore the loss on non-first tokens, or duplicate labels and collapse back to character level during decode

If implementation simplicity matters, another option is:

- force character-split input
- use `is_split_into_words=True` to preserve character boundaries

But in all cases, the document should not rely on the assumption that "Chinese is one character per token, so 512 tokens means 512 characters".

### 5.2 Original Position Mapping Must Be Preserved

Even if source quote characters are filtered out before inference, the system still needs:

- filtered character index -> original character index

This is required because the merge step must reinsert normalized quote marks back into the original content stream.

Recommended structure:

```python
filtered_chars: list[str]
original_indices: list[int]
```

not just a plain filtered string.

## 6. Data Construction

### 6.1 Ground Truth Sources

A practical three-stage data pipeline:

1. Generate initial normalized output from the current rule system
2. Correct it manually
3. Use active learning to add high-value samples

This is the lowest-cost way to bootstrap data.

### 6.2 Sample Priority

Prioritize annotation for:

- text that already contains quote marks, but with wrong direction or broken pairing
- text missing one quote boundary
- paragraph-internal nested quotes
- multi-paragraph dialogue
- text with only partial quote evidence, where recovery depends on context

Do not begin with a large amount of fully unquoted text that depends entirely on semantic judgment. That class is the most expensive and the most likely to create inconsistent labels.

### 6.3 Rough Data Scale

Approximate ranges:

- to validate the pipeline end to end: about `200` paragraphs
- to obtain a stable baseline: about `500 ~ 1000` paragraphs
- to significantly improve no-quote recovery: likely more than that

## 7. Long-Text Inference Strategy

### 7.1 Principle

Long paragraphs cannot simply be hard-truncated, because that would damage:

- span boundaries
- nested structure
- discourse continuity across multiple sentences

Recommended two-stage strategy:

1. Split by sentence boundaries whenever possible
2. Fall back to sliding windows only for extreme long-sentence cases

### 7.2 Revised Sentence Chunking Strategy

The original rule of "split once a sentence-ending mark appears after half the max length" is too rough.

A more stable method is:

1. Accumulate sentences
2. Stop when adding the next sentence would exceed the threshold
3. Split at the nearest sentence boundary

This produces fewer fragmented chunks.

### 7.3 Sliding Window Strategy

For extremely long single sentences, `emission`-level voting plus one final global CRF decode is reasonable.

Recommended:

- higher weights near the window center
- lower weights near the edges

But implementation should define clearly whether windows are built on:

- token indices
- or character indices

If supervision is character-level, aggregation should preferably resolve back to character level as well.

## 8. Post-Processing

### 8.1 Merge Strategy

The merge step should follow three rules:

1. discard all source quote characters
2. insert normalized quote marks only at predicted span boundaries
3. preserve all other source content as-is

This high-level logic is sound.

### 8.2 Invalid Label Repair

Even with a CRF, it is still worth keeping a light fallback repair pass for cases such as:

- `I1 / E1` without a preceding `B1`
- `I2 / E2` without a preceding `B2`

The repair should remain conservative and should not amplify model mistakes.

## 9. Export and C++ Inference

### 9.1 ONNX Export Boundary

Only export:

- `BERT + Linear`
- outputting `emissions`

Keeping CRF decode in C++ as handwritten Viterbi is the right boundary.

### 9.2 Implementation Detail That Must Be Fixed

The original draft had a clear bug in this class:

```python
class BertEmissionOnly(torch.nn.Module):
    def __init__(self, tagger):
        ...

    def forward(self, input_ids, attention_mask):
        return self.tagger.get_emissions(input_ids, attention_mask)
```

`self.tagger` is never assigned.

It should either:

```python
self.tagger = tagger
```

or inline the call directly inside `forward()`.

### 9.3 CRF Parameter Serialization

It is better not to use `torch.save(... .pt)` as the deployment interface for CRF transitions alone.

More C++-friendly options:

- `json`
- `npy`
- an explicit binary float array

This makes loading simpler and avoids dependence on PyTorch serialization at deployment time.

### 9.4 Tokenizer Deployment Cannot Be Left Implicit

One critical deployment point was missing from the original draft:

> How is tokenization handled on the C++ side?

If runtime inference is in C++, one of the following must be chosen explicitly:

1. use HuggingFace `tokenizers` through its C++ / Rust path
2. export the vocabulary and WordPiece rules and implement tokenization in C++
3. keep tokenization on the Python service side and expose ONNX inference through a service boundary

This must be decided early. Otherwise the "ONNX + C++ Viterbi" path is still incomplete.

## 10. Recommended Iteration Plan

### Stage 1: Build a Verifiable Baseline

- collect `200 ~ 500` training paragraphs
- focus first on samples that still contain damaged quote evidence
- run `BERT + CRF`
- complete the Python inference, ONNX export, and C++ emission + Viterbi path

Goals:

- validate the label design
- validate the deployment path

### Stage 2: Improve Complex Cases

- expand to `500 ~ 1000` paragraphs
- add active learning
- strengthen multi-paragraph dialogue and no-quote recovery samples
- compare `BERT + CRF` against `BERT + BiLSTM + CRF`

### Stage 3: Optimize for Very Long Text

- analyze real paragraph-length distributions from target novels
- tune sentence chunking and sliding-window parameters
- evaluate longer-context models only if necessary

## 11. Key Design Decisions

| Decision | Choice | Reason |
|---|---|---|
| Pretrained model | `hfl/chinese-roberta-wwm-ext` | mature Chinese base-scale model, stable and deployable |
| Label set | two-level BIOES (9 labels) | matches the fixed two-level quote structure |
| Input construction | filter source quotes before span labeling | more robust to noisy source punctuation |
| Supervision granularity | character-level labels with token alignment | avoids the false "one char = one token" assumption |
| Long-text strategy | sentence-boundary chunking + sliding-window fallback | preserve semantics first, preserve coverage second |
| Voting layer | emission-level voting | allows one final global CRF decode |
| C++ inference | ONNX Runtime + handwritten Viterbi | clean boundary between emissions and CRF decode |

## 12. Conclusion

This ML approach is viable, and it is better suited than pure rules for:

- heavily damaged source quotes
- no-quote recovery
- multi-paragraph dialogue
- context-sensitive ambiguity

Before implementation starts, a few points must be fixed and made explicit:

1. do not assume Chinese naturally maps to one token per character
2. preserve the mapping from filtered characters back to original positions
3. define the tokenizer deployment strategy
4. build a baseline on damaged-quote samples before attempting full no-quote recovery

If the project follows this revised plan, the implementation path will be more stable than directly following the original draft.
