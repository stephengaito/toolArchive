# the following code can be used to confirm that the
# XorShiftRNG code actually provides a "good enough"
# uniformly distributed collection of random numbers.

library(ecoNetworksSimulator)

# we start by initializing the RNG
model <- .C_newSpeciesTable(0)
.C_initRNGState(model,2,1000)

# now collect some results
# note that we collect a number of columns of data
# so that we can confirm how uniform each column
# of data is.
results <- matrix(0, nrow = 10000, ncol = 10)
results <- .C_getRandomNumbers(model, results)

# the following should be a nice straight line
plot(ecdf(results[,4]))

# alternatively the following *should* be a nice flat histogram
hist(results[,3], breaks = 100)

# alternative this should be a nice dense square (or nearly)
plot(results[,5])

# finally the [Kolmogorov–Smirnov test](https://en.wikipedia.org/wiki/Kolmogorov%E2%80%93Smirnov_test)
# should provide a very small D number with a high(?) p value
ks.test(results[,6], "punif")