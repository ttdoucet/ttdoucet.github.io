---
layout: default
title: Euler Index
---

## Euler original publications

I have curated and processed the following original publications
by Leonhard Euler, and made them permanently available on archive.org:
<br><br>

<ul class="EulerIndex">
{% for item in site.data.EulerIndex.EulerIndex %}
  <li>
    {{item[0]}}
    <a href="{{item[1].url}}">{{item[1].title}}</a>
    <br>
    <em style="font-size: 0.8em">{{item[1].citation}}</em><br>
  </li>
{% endfor %}
</ul>




