# Chinese Punctuation Rules (GB/T 15834-2011)

Reference for implementing `format/quotes.cpp` and related formatters.

---

## 1. Quotation Marks (引号)

### 1.1 Levels

- Primary: double quotation marks `" "` (U+201C, U+201D)
- Nested: single quotation marks `' '` (U+2018, U+2019)
- Rule: double outside, single inside.

```
"他说：'你听过"红楼梦"吗？'"
```

### 1.2 Multi-paragraph speech

When one person's speech spans multiple paragraphs:

- Each paragraph starts with `"`.
- Intermediate paragraphs do **not** get `"`.
- Only the **final** paragraph gets `"`.

```
"第一段话……
"第二段话……
"第三段话的结尾。"
```

### 1.3 Position

- Opening quote `"` must not appear at end of a line.
- Closing quote `"` must not appear at start of a line.

### 1.4 Parallel quoted items

No enumeration comma (、) between adjacent quotation-marked items.

---

## 2. Book Title Marks (书名号)

- Double: `《》` for books, periodicals, films, albums, software.
- Single: `〈〉` for nested titles.

```
《红楼梦》
《〈红楼梦〉研究》
```

---

## 3. Dashes (破折号 / 连接号)

| Name | Glyph | Width | Usage |
|------|-------|-------|-------|
| Em-dash | `——` | 2 em | Break in thought, explanation, range |
| En-dash (一字线) | `—` | 1 em | Ranges (numbers, dates) |
| Hyphen (短横线) | `-` | 0.5 em | Compounds, codes |
| Wave dash (浪纹线) | `～` | 1 em | Ranges (numbers) |

Rules:
- Em-dash `——` must not break across lines.
- Collapsed in `sanitize.cpp`: 4+ hyphens → `---`, 3+ tildes → `~~`.

---

## 4. Ellipsis (省略号)

- Standard form: `……` (U+2026, six dots, 2 em).
- Whole-paragraph omission: `…………` (12 dots, 4 em) on its own line.
- Must not break across lines.

Already handled in `sanitize.cpp`: 3 periods → `…`, 4+ → `……`.

---

## 5. Period / Question / Exclamation Marks (句号 / 问号 / 叹号)

- Full-width, 1 em.
- Must **not** appear at start of a line (prohibition rule).
- When inside a quote: if the quote is a complete sentence, punctuation inside `"`; if the quote is just a sentence fragment, punctuation outside `"`.

---

## 6. Interpunct (间隔号)

- `·` (U+00B7), 1 em, centered vertically.
- Used between: translated given/surname, book title and chapter, month and day in dates.

```
卡尔·马克思
《诗经·关雎》
一二·九运动
```

---

## 7. Emphasis Dot (着重号)

- `·` placed **below** the emphasized text (one dot per character).
- Rare in novels; usually only in textbooks/academic works.

```
着重号的标示方式
· ·
```

---

## 8. Proper Name Mark (专名号)

- A straight underline `____` below proper names (person, place, dynasty).
- Used in classical texts / vertical layout; rare in modern horizontal novels.

---

## 9. Separator (分隔号)

- `/` (U+002F), 0.5 em.
- Used for: poetry line breaks within prose, beat separation, parallel items.

```
床前明月光 / 疑是地上霜 / 举头望明月 / 低头思故乡
```

---

## 10. Formatting implications for novelfmt

### In `sanitize.cpp` (already done)

- Space / fullwidth space / NBSP removal.
- `\r\n` → `\n`, consecutive `\n` → single `\n`.
- 4+ hyphens → `---`, 3+ tildes → `~~`.
- Period → ellipsis conversion.

### In `format/quotes.cpp` (to implement)

- Convert typewriter quotes `"` `'` to typographic `"` `'` when inside Chinese text.
- Detect multi-paragraph speech: a line starting with `"` but the previous
  speech block was not closed → this is a continuation.
- Validate/repair unbalanced quotes within a speech block.
- Opening/closing quote position rules (line-breaking prohibitions).

### In `format/heading.cpp` (to consider)

- Book title marks `《》` normalization (e.g., unify half-width `< >` → `〈〉`).
- Interpunct `·` normalization in chapter titles.

### Option-driven

- `normalize_line_endings`: enable line-breaking prohibition checks
  (period/question/exclamation at line start → pull up to previous line).
- `normalize_quotes`: all quotation mark normalization.
