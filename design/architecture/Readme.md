# Ecological Networks Simulator -- Design

## Problem

Simulate relatively realistic spatially extended trophic networks.

The objective is not to explicitly model ecological systems in 
themselves, but rather to provide a platform with which to synthetically 
produce large reasonably realistic data sets from which to observe the 
influence of different underlying assumptions of both network structure 
and data collection.

## Goals

Provide a simple system with which to generate large synthetic datasets 
of reasonably realistic trophic models.

Provide a simple system to observe the possible effects of spatial and 
trophic structure.

## Requirements

> Provide an R-language extension to ensure all data can be specified and 
> used inside normal R-language scripts.

> Provide tools to specify trophic structure.

> Provide tools to specify spatial structure.

> Provide tools to choose parameters from simple (normal) statistical 
> distributions.

## Solution

## Questions and Risks

## Resources

It is "well known" (in the PCA/dynamical-systems folk lore) that 
rectangular spatial structure imposes additional implicit structure on 
the solutions, so we should consider using [hexagonal spatial 
cells](http://www.redblobgames.com/grids/hexagons/).

[R-language manuals](https://cran.r-project.org/manuals.html)

[Writing R 
Extensions](https://cran.r-project.org/doc/manuals/r-release/R-exts.html)

[R language](https://www.r-project.org/)

[An Introduction to the .C Interface to 
R](http://www.biostat.jhsph.edu/~rpeng/docs/interface.pdf)

[Advanced R](http://adv-r.had.co.nz/)

[R packages](http://r-pkgs.had.co.nz/)

We will use [devtools](https://github.com/hadley/devtools) 
[roxygen2](https://cran.r-project.org/web/packages/roxygen2/vignettes/roxygen2.html) 
[RMarkdown v2](http://rmarkdown.rstudio.com/) 
[knitr](https://github.com/yihui/knitr) and 
[testthat](https://github.com/hadley/testthat)

[MathJaX](https://www.mathjax.org/) [TeX and LaTeX 
support](http://docs.mathjax.org/en/latest/tex.html#tex-support)
