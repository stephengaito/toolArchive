# Spatial integration

## Problem

We would ultimately like to be able to study the spatial behaviour using 
spatial integration techinques from Mathematical Physics. 

It is highly likely that spatial extension of the normal logistic 
equation is best thought of as a 
[diffusion](https://en.wikipedia.org/wiki/Diffusion_equation) in space.

Since the logistic equations themselves are non-linear, we have an active 
(non-linear) diffusive system. (see "Numerical Recipes in C" section 
17.2).

The Courant stability analysis actually suggests that we end up with 
something like a [Navier-Stokes 
equation](https://en.wikipedia.org/wiki/Navier%E2%80%93Stokes_equations).

## Goals

## Requirements

## Solution

## Questions and Risks

It is important to realize that the "spurious" solutions to a non-linear 
diffusion equation with sources put rather strict limits on the spatial 
and time integration steps.  Critically the step size must encompase the 
information propogation ("light") cone. (See "Numerical Recipes in C" 
section 17.2)

## Resources

"Numerical Recipes in C" section 17.2 "Diffusive Initial value problems"
