# CMDC — CommonMark to HTML parser

Легковесный партер Markdown, написанный на Си. 
Реализует спецификацию CommonMark v0.31.2, используя подход конечного автомата 
и стека тегов для обработки вложенных структур.

### Статус поддержки CommonMark (v0.31.2)

Условные обозначения: 
- `→` означает символ табуляции,
- `·` означает символ пробела.

- [x] Example 1: `→foo→baz→→bim` -> `<pre><code>foo→baz→→bim</code></pre>`
- [x] Example 2: `··→foo→baz→→bim` -> `<pre><code>foo→baz→→bim</code></pre>`
- [x] Example 3: 
    ```
    ····a→a
    ····ὐ→a
    ```
    -> 
    ```
    <pre><code>a→a
    ὐ→a
    </code></pre>
    ```