---
layout: default
title: Euler Index
---

## Leonhard Euler original publications

I have processed the following original publications
by Leonhard Euler, and made them permanently available on archive.org.
I have also [translated][euler-translations] several of them.

<div class="EulerIndex">
<ul>
{% for item in site.data.EulerIndex.EulerIndex %}
  <li>
    {{item[0]}}
    <a href="{{item[1].url}}"><cite>{{item[1].title}}</cite></a>
    <br>
    <journal>{{item[1].citation | smartify}}</journal>
  </li>
{% endfor %}
</ul>
</div>

[euler-translations]: /euler

