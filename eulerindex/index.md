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
  {% assign enumber = item[0] %}
  {% assign edata = item[1] %}
  <li>
    <a href="https://archive.org/details/euler-{{ enumber }}">{{ enumber }}</a>
    <a href="https://archive.org/download/euler-{{ enumber }}/euler-{{ enumber }}.pdf"><cite>{{ edata.title }}</cite></a>
    <br>
    <span class="journal">{{ edata.citation | smartify }}</span>
  </li>
{% endfor %}
</ul>
</div>

[euler-translations]: /euler



