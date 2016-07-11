# Models

## Problem

A user must be able to build complex spatially extended trophic models 
which contain multiple species and potentially multiple spatial types.

## Goals

The interaction between the ecoNetworksSimulator and the R environment 
must be conceptually clean.

Creation of models should be relatively easy.

Simulation of models should be performant.

## Requirements

> Simulation of models MUST be performant.

> Tasks which are not particularly long running or time critical SHOULD 
> be done using R scripts.

## Solution

Model structure and parameters will be shadowed in C structures. This 
means that, since everything is in (less performant) R objects, models 
can be saved and re-loaded by simply creating the shadowed C structures 
as each model object is re-loaded.

Models, species, etc will be first class R objects which have [External 
pointers](https://cran.r-project.org/doc/manuals/r-release/R-exts.html#External-pointers-and-weak-references) 
to corresponding C structures which contain the structure and parameters 
required to simulate the models. Each Simulator external pointer will 
have a corresponding finalizer to clean up the C structures when any 
parts of the model structure are garbage collected by R.

The simulator will have a number of profiling values which can be used to 
detect if a given finalizer has been called.

To be able to hold both a C structure as well as R struture, the Model 
objects presumably have to be S3, S4 or RC objects. On the "advice" of 
[OO field guide](http://adv-r.had.co.nz/OO-essentials.html) and [Picking 
an OO 
system](http://adv-r.had.co.nz/OO-essentials.html#picking-a-system), we 
will use simple S3 objects.

## Questions and Risks

How is a model serialized and re-loaded?

The most critical problem faced during simulation will be storing each 
computed value into a four dimensional R 
[array](http://adv-r.had.co.nz/Data-structures.html#matrices-and-arrays) 
structure (x and y spatial dimensions, time dimension, species 
dimension).

To see how to access a simulation's output array see 
[data.table](https://github.com/Rdatatable/data.table) and 
[dplyr](https://github.com/hadley/dplyr).  Note, while not explicitly 
stated, since the Garbage Collector is generational, we have to assume 
that *any* R object's pointer might be changed during a GC. This means 
that we need to re-obtain our pointers after each check for R-interupts. 
We will assume that during our own integration step we WILL NOT yield to 
R so that R's GC will not be run (this might be a dangerous assumption).

One solution to the shadowing problem might be to keep all model 
structure in R objects but to build fast local model structure shadows at 
the begining of any simulation and release this shadow at the end of a 
simulation.

## Resources

[R Internals](https://cran.r-project.org/doc/manuals/r-release/R-ints.html)

[Rinternals.h](https://github.com/SurajGupta/r-source/blob/master/src/include/Rinternals.h)

[R-source 
include](https://github.com/SurajGupta/r-source/tree/master/src/include)
