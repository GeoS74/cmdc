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
- [x] Example 11: `*→*→*→` -> `<hr />`
### Backslash escapes
- [x] Example 12:
    ```
    \!\"\#\$\%\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\?\@\[\\\]\^\_\`\{\|\}\~
    ```
    -> 
    ```
    <p>!&quot;#$%&amp;'()*+,-./:;&lt;=&gt;?@[\]^_`{|}~</p>
    ```
- [x] Example 13:
    `\→\A\a\ \3\φ\«` -> `<p>\→\A\a\ \3\φ\«</p>`
- [x] Example 16:
    ```
    foo\
    bar
    ```
    -> 
    ```
    <p>foo<br />
    bar</p>
    ```
- [x] Example 17: 
    ```
    ``·\[\`·``
    ``` 
    -> 
    ```
    <p><code>\[\`</code></p>
    ```
- [x] Example 18: `····\[\]` -> 
    ```
    <pre><code>\[\]
    </code></pre>
    ```
### Thematic breaks
- [x] Example 43:
    ```
    ***
    ---
    ___
    ```
    ->
    ```
    <hr />
    <hr />
    <hr />
    ```
- [x] Example 44: `+++` -> `<p>+++</p>`
- [x] Example 45: `===` -> `<p>===</p>`
- [x] Example 46:
    ```
    --
    **
    __
    ```
    ->
    ```
    <p>--
    **
    __</p>
    ```
- [x] Example 47:
    ```
    ·***
    ··***
    ···***
    ```
    ->
    ```
    <hr />
    <hr />
    <hr />
    ```
- [x] Example 48:
    ```
    ····***
    ```
    ->
    ```
    <pre><code>***
    </code></pre>
    ```
- [x] Example 49:
    ```
    Foo
    ····***
    ```
    ->
    ```
    <p>Foo
    ***</p>
    ```
- [x] Example 50:
    ```
    _____________________________________
    ```
    ->
    ```
    <hr />
    ```
- [x] Example 51: `·-·-·-` -> `<hr />`
- [x] Example 52: `·**··*·**·*·**·*·**` -> `<hr />`
- [x] Example 53: `-·····-······-······-` -> `<hr />`
- [x] Example 54: `-·-·-·-····` -> `<hr />`
- [x] Example 55:
    ```
    _·_·_·_·a

    a------

    ---a---
    ```
    ->
    ```
    <p>_·_·_·_·a</p>
    <p>a------</p>
    <p>---a---</p>
    ```
- [x] Example 58:
    ```
    Foo
    ***
    bar
    ```
    ->
    ```
    <p>Foo</p>
    <hr />
    <p>bar</p>
    ```
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
- [x] Example 65:
    ```
    \##·foo
    ```
    ->
    ```
    <p>##·foo</p>
    ```
- [ ] Example 66:
    ```
    #·foo·*bar*·\*baz\*
    ```
    ->
    ```
    <h1>foo·<em>bar</em>·*baz*</h1>
    ```
- [x] Example 67:
    ```
    #··················foo·····················
    ```
    ->
    ```
    <h1>foo</h1>
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
    ····#·foo
    ```
    ->
    ```
    <pre><code>#·foo
    </code></pre>
    ```
- [x] Example 70:
    ```
    foo
    ····#·bar
    ```
    ->
    ```
    <p>foo
    #·bar</p>
    ```
- [x] Example 71:
    ```
    ##·foo·##
    ··###···bar····###
    ```
    ->
    ```
    <h2>foo</h2>
    <h3>bar</h3>
    ```
- [x] Example 72:
    ```
    #·foo·##################################
    #####·foo·##
    ```
    ->
    ```
    <h1>foo</h1>
    <h5>foo</h5>
    ```
- [x] Example 73:
    ```
    ###·foo·###·····
    ```
    ->
    ```
    <h3>foo</h3>
    ```
- [x] Example 74:
    ```
    ###·foo·###·b
    ```
    ->
    ```
    <h3>foo·###·b</h3>
    ```
- [x] Example 75: `#·foo#` -> `<h1>foo#</h1>`
- [x] Example 76:
    ```
    ###·foo·\###
    ##·foo·#\##
    #·foo·\#
    ```
    ->
    ```
    <h3>foo·###</h3>
    <h2>foo·###</h2>
    <h1>foo·#</h1>
    ```
- [x] Example 77:
    ```
    ****
    ##·foo
    ****
    ```
    ->
    ```
    <hr />
    <h2>foo</h2>
    <hr />
    ```
- [x] Example 78:
    ```
    Foo·bar
    #·baz
    Bar·foo
    ```
    ->
    ```
    <p>Foo·bar</p>
    <h1>baz</h1>
    <p>Bar·foo</p>
    ```
- [x] Example 79:
    ```
    ##·
    #
    ###·###
    ```
    ->
    ```
    <h2></h2>
    <h1></h1>
    <h3></h3>
    ```
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
### Fenced code blocks
- [x] Example 119:
    ````
    ```
    <
    ·>
    ```
    ````
    ->
    ```
    <pre><code>&lt;
    ·&gt;
    </code></pre>
    ```
- [x] Example 120:
    ````
    ~~~
    <
    ·>
    ~~~
    ````
    ->
    ```
    <pre><code>&lt;
    ·&gt;
    </code></pre>
    ```
- [x] Example 121:
    ````
    ``
    foo
    ``
    ````
    ->
    ```
    <p><code>foo</code></p>
    ```
- [x] Example 122:
    ````
    ```
    aaa
    ~~~
    ```
    ````
    ->
    ```
    <pre><code>aaa
    ~~~
    </code></pre>
    ```
- [x] Example 123:
    ````
    ~~~
    aaa
    ```
    ~~~
    ````
    ->
    ````
    <pre><code>aaa
    ```
    </code></pre>
    ````
- [x] Example 124:
    ~~~
    ````
    aaa
    ```
    ``````
    ~~~
    ->
    ````
    <pre><code>aaa
    ```
    </code></pre>
    ````
- [x] Example 125:
    ```
    ~~~~
    aaa
    ~~~
    ~~~~
    ```
    ->
    ```
    <pre><code>aaa
    ~~~
    </code></pre>
    ```
- [x] Example 126:
    ````
    ```
    ````
    ->
    ```
    <pre><code></code></pre>
    ```
- [x] Example 127:
    ~~~
    `````

    ```
    aaa
    ~~~
    ->
    ````
    <pre><code>
    ```
    aaa
    </code></pre>
    ````
- [ ] Example 128:
    ~~~
    >·```
    >·aaa

    bbb
    ~~~
    ->
    ````
    <blockquote>
    <pre><code>aaa
    </code></pre>
    </blockquote>
    <p>bbb</p>
    ````
- [x] Example 129:
    ~~~
    ```

    ··
    ```
    ~~~
    ->
    ````
    <pre><code>
    ··
    </code></pre>
    ````
- [x] Example 130:
    ~~~
    ```
    ```
    ~~~
    ->
    ````
    <pre><code></code></pre>
    ````
- [x] Example 131:
    ~~~
    ·```
    ·aaa
    aaa
    ```
    ~~~
    ->
    ````
    <pre><code>aaa
    aaa
    </code></pre>
    ````
- [x] Example 132:
    ~~~
    ··```
    aaa
    ··aaa
    aaa
    ··```
    ~~~
    ->
    ````
    <pre><code>aaa
    aaa
    aaa
    </code></pre>
    ````
- [x] Example 133:
    ~~~
    ···```
    ···aaa
    ····aaa
    ··aaa
    ···```
    ~~~
    ->
    ````
    <pre><code>aaa
    ·aaa
    aaa
    </code></pre>
    ````
- [x] Example 134:
    ~~~
    ····```
    ····aaa
    ····```
    ~~~
    ->
    ````
    <pre><code>```
    aaa
    ```
    </code></pre>
    ````
- [x] Example 135:
    ~~~
    ```
    aaa
    ··```
    ~~~
    ->
    ````
    <pre><code>aaa
    </code></pre>
    ````
- [x] Example 136:
    ~~~
    ···```
    aaa
    ··```
    ~~~
    ->
    ````
    <pre><code>aaa
    </code></pre>
    ````
- [x] Example 137:
    ~~~
    ```
    aaa
    ····```
    ~~~
    ->
    ````
    <pre><code>aaa
    ····```
    </code></pre>
    ````
- [x] Example 138:
    ~~~
    ```·```
    aaa
    ~~~
    ->
    ````
    <p><code>·</code>
    aaa</p>
    ````
- [x] Example 139:
    ```
    ~~~~~~
    aaa
    ~~~·~~
    ```
    ->
    ````
    <pre><code>aaa
    ~~~·~~
    </code></pre>
    ````
- [x] Example 140:
    ~~~
    foo
    ```
    bar
    ```
    baz
    ~~~
    ->
    ````
    <p>foo</p>
    <pre><code>bar
    </code></pre>
    <p>baz</p>
    ````
- [ ] Example 141:
    ~~~~~
    foo
    ---
    ~~~
    bar
    ~~~
    # baz
    ~~~~~
    ->
    ````
    <h2>foo</h2>
    <pre><code>bar
    </code></pre>
    <h1>baz</h1>
    ````
- [x] Example 142:
    ~~~
    ```ruby
    def·foo(x)
    ··return 3
    end
    ```
    ~~~
    ->
    ````
    <pre><code class="language-ruby">def·foo(x)
    ··return 3
    end
    </code></pre>
    ````
- [x] Example 143:
    ```
    ~~~~····ruby·startline=3·$%@#$
    def·foo(x)
    ··return·3
    end
    ~~~~~~~
    ```
    ->
    ````
    <pre><code class="language-ruby">def·foo(x)
    ··return·3
    end
    </code></pre>
    ````
- [x] Example 144:
    ~~~
    ````;
    ````
    ~~~
    ->
    ````
    <pre><code class="language-;"></code></pre>
    ````
- [x] Example 145:
    ~~~
    ```·aa·```
    foo
    ~~~
    ->
    ````
    <p><code>aa</code>
    foo</p>
    ````
- [x] Example 146:
    ```
    ~~~·aa·```·~~~
    foo
    ~~~
    ```
    ->
    ````
    <pre><code class="language-aa">foo
    </code></pre>
    ````
- [x] Example 147:
    ~~~
    ```
    ```·aaa
    ```
    ~~~
    ->
    ````
    <pre><code>```·aaa
    </code></pre>
    ````
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
- [x] Example 226:
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
### Block quotes
- [x] Example 228:
    ```
    >·#·Foo
    >·bar
    >·baz
    ```
    ->
    ```
    <blockquote>
    <h1>Foo</h1>
    <p>bar
    baz</p>
    </blockquote>
    ```
- [x] Example 229:
    ```
    >#·Foo
    >bar
    >·baz
    ```
    ->
    ```
    <blockquote>
    <h1>Foo</h1>
    <p>bar
    baz</p>
    </blockquote>
    ```
- [x] Example 230:
    ```
    ···>·#·Foo
    ···>·bar
    ·>·baz
    ```
    ->
    ```
    <blockquote>
    <h1>Foo</h1>
    <p>bar
    baz</p>
    </blockquote>
    ```
- [x] Example 231:
    ```
    ····>·#·Foo
    ····>·bar
    ····>·baz
    ```
    ->
    ```
    <pre><code>&gt; # Foo
    &gt; bar
    &gt; baz
    </code></pre>
    ```
- [x] Example 232:
    ```
    >·#·Foo
    >·bar
    baz
    ```
    ->
    ```
    <blockquote>
    <h1>Foo</h1>
    <p>bar
    baz</p>
    </blockquote>
    ```
- [x] Example 233:
    ```
    >·bar
    baz
    >·foo
    ```
    ->
    ```
    <blockquote>
    <p>bar
    baz
    foo</p>
    </blockquote>
    ```
- [x] Example 234:
    ```
    >·foo
    ---
    ```
    ->
    ```
    <blockquote>
    <p>foo</p>
    </blockquote>
    <hr />
    ```
- [ ] Example 235:
    ```
    >·-·foo
    -·bar
    ```
    ->
    ```
    <blockquote>
    <ul>
    <li>foo</li>
    </ul>
    </blockquote>
    <ul>
    <li>bar</li>
    </ul>
    ```
- [x] Example 236:
    ```
    >····foo
    ····bar
    ---
    ```
    ->
    ```
    <blockquote>
    <pre><code>foo
    </code></pre>
    </blockquote>
    <pre><code>bar
    </code></pre>
    ```


### Code spans
- [x] Example 327:
    ```
    `hi`lo`
    ```
    ->
    ```
    <p><code>hi</code>lo`</p>
    ```
- [x] Example 328:
    ```
    `foo`
    ```
    ->
    ```
    <p><code>foo</code></p>
    ```
- [x] Example 329:
    ```
    ``·foo·`·bar·``
    ```
    ->
    ```
    <p><code>foo·`·bar</code></p>
    ```
- [x] Example 330:
    ```
    `·``·`
    ```
    ->
    ```
    <p><code>``</code></p>
    ```
- [x] Example 331:
    ```
    `··``··`
    ```
    ->
    ```
    <p><code>·``·</code></p>
    ```
- [x] Example 332:
    ```
    `·a`
    ```
    ->
    ```
    <p><code>·a</code></p>
    ```
- [x] Example 333:
    ```
    ` b `
    ```
    ->
    ```
    <p><code> b </code></p>
    ```
- [x] Example 334:
    ```
    `·`
    `··`
    ```
    ->
    ```
    <p><code>·</code>
    <code>··</code></p>
    ```
- [x] Example 335:
    ```
    ``
    foo
    bar··
    baz
    ``
    ```
    ->
    ```
    <p><code>foo·bar···baz</code></p>
    ```
- [x] Example 336:
    ```
    ``
    foo·
    ``
    ```
    ->
    ```
    <p><code>foo·</code></p>
    ```
- [x] Example 337:
    ```
    `foo···bar·
    baz`
    ```
    ->
    ```
    <p><code>foo···bar··baz</code></p>
    ```
- [x] Example 338:
    ```
    `foo\`bar`
    ```
    ->
    ```
    <p><code>foo\</code>bar`</p>
    ```
- [x] Example 339:
    ```
    ``foo`bar``
    ```
    ->
    ```
    <p><code>foo`bar</code></p>
    ```
- [x] Example 340:
    ```
    `·foo·``·bar·`
    ```
    ->
    ```
    <p><code>foo·``·bar</code></p>
    ```
- [x] Example 341:
    ```
    *foo`*`
    ```
    ->
    ```
    <p>*foo<code>*</code></p>
    ```
- [x] Example 342:
    ```
    [not·a·`link](/foo`)
    ```
    ->
    ```
    <p>[not·a·<code>link](/foo</code>)</p>
    ```
- [x] Example 343:
    ```
    `<a href="`">`
    ```
    ->
    ```
    <p><code>&lt;a href=&quot;</code>&quot;&gt;`</p>
    ```
- [ ] Example 344:
    ```
    <a href="`">`
    ```
    ->
    ```
    <p><a href="`">`</p>
    ```
- [x] Example 345:
    ```
    `<https://foo.bar.`baz>`
    ```
    ->
    ```
    <p><code>&lt;https://foo.bar.</code>baz&gt;`</p>
    ```
- [ ] Example 346:
    ```
    <https://foo.bar.`baz>`
    ```
    ->
    ```
    <p><a href="https://foo.bar.%60baz">https://foo.bar.`baz</a>`</p>
    ```
- [x] Example 347:
    ```
    ```foo``
    ```
    ->
    ```
    <p>```foo``</p>
    ```
- [x] Example 348:
    ```
    `foo
    ```
    ->
    ```
    <p>`foo</p>
    ```
- [x] Example 349:
    ```
    `foo``bar``
    ```
    ->
    ```
    <p>`foo<code>bar</code></p>
    ```

### Hard line breaks
- [x] Example 633:
    ```
    foo··
    baz
    ```
    ->
    ```
    <p>foo<br />
    baz</p>
    ```
- [x] Example 634:
    ```
    foo\
    baz
    ```
    ->
    ```
    <p>foo<br />
    baz</p>
    ```
- [x] Example 635:
    ```
    foo·······
    baz
    ```
    ->
    ```
    <p>foo<br />
    baz</p>
    ```
- [x] Example 636:
    ```
    foo··
    ·····baz
    ```
    ->
    ```
    <p>foo<br />
    baz</p>
    ```
- [x] Example 637:
    ```
    foo\
    ·····baz
    ```
    ->
    ```
    <p>foo<br />
    baz</p>
    ```
- [x] Example 644: `foo\` -> `<p>foo\</p>`
- [x] Example 645: `foo··` -> `<p>foo</p>`
- [x] Example 646: `###·foo\` -> `<h3>foo\</h3>`
- [x] Example 647: `###·foo··` -> `<h3>foo\</h3>`
### Soft line breaks
- [x] Example 648:
    ```
    foo
    baz
    ```
    ->
    ```
    <p>foo
    baz</p>
    ```
- [x] Example 649:
    ```
    foo·
    ·baz
    ```
    ->
    ```
    <p>foo
    baz</p>
    ```
### Textual content
- [x] Example 650: `hello·$.;'there` -> `<p>hello·$.;'there</p>`
- [x] Example 651: `Foo·χρῆν` -> `<p>Foo·χρῆν</p>`
- [x] Example 652: `Multiple·····spaces` -> `<p>Multiple·····spaces</p>`
