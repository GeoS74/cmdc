### Статус поддержки CommonMark (v0.31.2)

Условные обозначения: 
- `→` означает символ табуляции,
- `·` означает символ пробела.

- [x] Example 1: `→foo→baz→→bim` -> 
    ```
    <pre><code>foo→baz→→bim
    </code></pre>
    ```
- [x] Example 2: `··→foo→baz→→bim` -> 
    ```
    <pre><code>foo→baz→→bim
    </code></pre>
    ```
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
- [x] Example 62:
    ```
    #·foo
    ##·foo
    ###·foo
    ####·foo
    #####·foo
    ######·foo
    ```
    ->
    ```
    <h1>foo</h1>
    <h2>foo</h2>
    <h3>foo</h3>
    <h4>foo</h4>
    <h5>foo</h5>
    <h6>foo</h6>
    ```
- [x] Example 63: `#######·foo` -> `<p>#######·foo</p>`
