# Sprint 001 Build trophic model

* Started: 2016-07-08
* Ends: 2016-07-15

## Problem

We need to be able to build simple trophic models with no spatial extent.

We would also like to run them using both the crude Euler and rather more 
refined fourth-order Runge-Kutta methods.

Finally we would like to be able to output the current trophic model as a 
set of differential equations in a LaTeX format.

## Tasks

* add model
* add species
* add species interactions

## Features

## Specifications

## Questions and Risks

With spatial extension of these models, fourth-order Runge-Kutta models 
may no longer be relevant. (See "Numerical Recipes in C" section 17.2). 
The critical problem is to find appropriate spatial derivative methods 
which include information beyond the "mobility" limit of the fastest 
moving species. It is this fastest moving species which will define the 
spatial and time integration scales.

## Resources

## Wrap-up

Spatially extended models will be the subject of some future sprints.
