---
layout: default
title: Fingerprints
---

[repo]: https://github.com/ttdoucet/image

# Fingerprints

I wrote a simple but fast image processing library to facilitate
investigations in image enhancement.  The library is written in
C# and runs under the .NET framework.

The generic library code, along with code for a simple GUI to feed images and see the results
of various computations, is in this [repo][Repo].

All of the computations below involve separable filter kernels and
should be easily implementable at video speeds on embedded hardware.

<div class="imgfig">
<figure>
  <img src="pics/input.png" />
  <figcaption>Original snapshot of finger showing fingerprint.</figcaption>
</figure>
</div>


<div class="imgfig">
<figure>
  <img src="pics/gradient.png" />
  <figcaption>Gradient computation showing tangents & normals.</figcaption>
</figure>
</div>

<div class="imgfig">
<figure>
  <img src="pics/output.png" />
  <figcaption>Gaussian blur followed by Laplacian, then
              a specialized thresholding.
  </figcaption>
</figure>
</div>

<div class="imgfig">
<figure>
  <img src="pics/fmask.png" />
  <figcaption>Strength of computed orientation field
              allows us to mask off areas of the
              image that are not of interest.
  </figcaption>
</figure>
</div>

<div class="imgfig">
<figure>
  <img src="pics/zebra.png" />
  <figcaption>
    This stable image results from repeatedly
    feeding back the output image as input.
  </figcaption>
</figure>
</div>

That the image is a fixed point of this iterated function system
suggests that it might be useful to make contraction mappings
based on the original input, as a completely different way to
proceed.
