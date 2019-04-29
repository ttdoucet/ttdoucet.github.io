---
layout: default
title: Euler Index
---

## Euler original publications

I have processed the following original publications
by Leonhard Euler, and made them permanently available on archive.org.
I have also [translated][euler-translations] several of them.
<br><br>

<ul class="EulerIndex">
{% for item in site.data.EulerIndex.EulerIndex %}
  <li>
    {{item[0]}}
    <a href="{{item[1].url}}"><cite>{{item[1].title}}</cite></a><br>
    <em style="font-size: 0.8em">{{item[1].citation | smartify}}</em><br>
  </li>
{% endfor %}
</ul>


[euler-translations]: /euler

