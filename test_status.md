# Статус поддержки CommonMark (v0.31.2)

Условные обозначения: 
- `→` означает символ табуляции,
- `·` означает символ пробела.

### Tabs
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
- [x] Example 8:
    ```
    ····foo
    →bar
    ```
    -> 
    ```
    <pre><code>foo
    bar
    </code></pre>
    ```
- [x] Example 10: `#→Foo` -> `<h1>Foo</h1>`
### ATX headings
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
- [x] Example 64:
    ```
    #5 bolt

    #hashtag
    ```
    ->
    ```
    <<p>#5 bolt</p>
    <p>#hashtag</p>
    ```
- [x] Example 68:
    ```
    ·###·foo
    ··##·foo
    ···#·foo
    ```
    ->
    ```
    <h3>foo</h3>
    <h2>foo</h2>
    <h1>foo</h1>
    ```
- [x] Example 69: `····#·foo` -> 
    ```
    <pre><code># foo
    </code></pre>
    ```
- [x] Example 75: `#·foo#` -> `<h1>foo#</h1>`
### Indented code blocks
- [x] Example 107:
    ```
    ····a simple
    ······indented code block
    ```
    ->
    ```
    <pre><code>a simple
      indented code block
    </code></pre>
    ```
- [x] Example 111:
    ```
    ····chunk1

    ····chunk2
    ··
    ·
    ·
    ····chunk3
    ```
    ->
    ```
    <pre><code>chunk1

    chunk2



    chunk3
    </code></pre>
    ```
- [x] Example 112:
    ```
    ····chunk1
    ······
    ······chunk2
    ```
    ->
    ```
    <pre><code>chunk1
  
      chunk2
    </code></pre>
    ```
- [x] Example 113:
    ```
    Foo
    ····bar
    ```
    ->
    ```
    <p>Foo
    bar</p>
    ```
- [x] Example 114:
    ```
    ····foo
    bar
    ```
    ->
    ```
    <pre><code>foo
    </code></pre>
    <p>bar</p>
    ```
- [ ] Example 115:
    ```
    # Heading
    ····foo
    Heading
    ------
    ····foo
    ----
    ```
    ->
    ```
    <h1>Heading</h1>
    <pre><code>foo
    </code></pre>
    <h2>Heading</h2>
    <pre><code>foo
    </code></pre>
    <hr />
    ```
- [x] Example 116:
    ```
    ········foo
    ····bar
    ```
    ->
    ```
    <pre><code>    foo
    bar
    </code></pre>
    ```
- [x] Example 117:
    ```

    ····
    ····foo
    ····
    ```
    ->
    ```
    <pre><code>foo
    </code></pre>
    ```
- [x] Example 118:
    ```
    ····foo··
    ```
    ->
    ```
    <pre><code>foo  
    </code></pre>
    ```
### Paragraphs
- [x] Example 219:
    ```
    aaa
    
    bbb
    ```
    ->
    ```
    <p>aaa</p>
    <p>bbb</p>
    ```
- [x] Example 220:
    ```
    aaa
    bbb

    ccc
    ddd
    ```
    ->
    ```
    <p>aaa
    bbb</p>
    <p>ccc
    ddd</p>
    ```
- [x] Example 221:
    ```
    aaa

    bbb
    ```
    ->
    ```
    <p>aaa</p>
    <p>bbb</p>
    ```
- [x] Example 222:
    ```
    ··aaa
    ·bbb
    ```
    ->
    ```
    <p>aaa
    bbb</p>
    ```
- [x] Example 223:
    ```
    aaa
    ·············bbb
    ·······································ccc
    ```
    ->
    ```
    <p>aaa
    bbb
    ccc</p>
    ```
- [x] Example 224:
    ```
    ···aaa
    bbb
    ```
    ->
    ```
    <p>aaa
    bbb</p>
    ```
- [x] Example 225:
    ```
    ····aaa
    bbb
    ```
    ->
    ```
    <pre><code>aaa
    </code></pre>
    <p>bbb</p>
    ```
- [ ] Example 226:
    ```
    aaa·····
    bbb·····
    ```
    ->
    ```
    <p>aaa<br />
    bbb</p>
    ```
### Blank lines
- [x] Example 227:
    ```
      
    
    aaa
      
    
    # aaa
    
      
    ```
    ->
    ```
    <p>aaa</p>
    <h1>aaa</h1>
    ```
