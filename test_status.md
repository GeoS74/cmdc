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
- [ ] Example 4:
    ```
    ··-·foo

    →bar
    ```
    ->
    ```
    <ul>
    <li>
    <p>foo</p>
    <p>bar</p>
    </li>
    </ul>
    ```