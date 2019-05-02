---
layout: default
title: Euler Index
---

## Leonhard Euler Originals

I have processed the following original publications
by Leonhard Euler, and made them permanently available on archive.org.
I have also [translated][euler-translations] several of them.

Click on the e-number to go to the reader on archive.org, or
the title to directly download the PDF.

<div>
<ul class="EulerIndex">
{% for item in site.data.EulerIndex.EulerIndex %}
  <li>
    <a href="https://archive.org/details/euler-{{item[0]}}">{{item[0]}}</a>
    <a href="https://archive.org/download/euler-{{item[0]}}/euler-{{item[0]}}.pdf"><cite>{{item[1].title}}</cite></a>
    <br>
    <span class="journal">{{item[1].citation | smartify}}</span>
  </li>
{% endfor %}
</ul>
</div>

[euler-translations]: /euler



