context("Test the Random Number Generator")

test_that(".C_initRNGState can initialize RNG state", {
  cModel <- .C_newSpeciesTable(0)
  expect_true(.C_initRNGState(cModel, 1, 0))
  rngState <- .C_getRNGState(cModel)
  expectedValue <- 0
  for (i in 1:33) {
    expect_equal(rngState[i], expectedValue)
    if (expectedValue == 0) {
      expectedValue <- 1
    } else {
      expectedValue <- 0
    }
  }
  expect_true(.C_initRNGState(cModel, 1, 1000))
  rngState <- .C_getRNGState(cModel)
  expectedValue <- 0
  for (i in 1:33) {
    expect_false(isTRUE(all.equal(rngState[i], expectedValue)))
    if (expectedValue == 0) {
      expectedValue <- 1
    } else {
      expectedValue <- 0
    }
  }
  expect_true(.C_initRNGState(cModel, 2, 0))
  rngState <- .C_getRNGState(cModel)
  expect_equal(rngState[1], 0)
  expect_false(isTRUE(all.equal(rngState[2], 0)))
  for (i in 3:33) {
    expect_equal(rngState[i], 0)
  }
})

test_that("can set/get RNG state", {
  cModel <- .C_newSpeciesTable(0)
  rngState <- c(1)
  for (i in 2:33) {
    rngState <- c(rngState, i)
  }
  expect_true(.C_setRNGState(cModel, rngState))
  rngState <- .C_getRNGState(cModel)
  for (i in 1:33) {
    expect_equal(rngState[i], i)
  }
})

test_that("get a random number between 0 and 1", {
  cModel <- .C_newSpeciesTable(0)
  rngState <- c(1)
  for (i in 2:33) {
    rngState <- c(rngState, i)
  }
  expect_true(.C_setRNGState(cModel, rngState))
  for (i in 1:500) {
    .C_getARandomNumber(cModel)
  }
  rnd <- .C_getARandomNumber(cModel)
  expect_true(all.equal(rnd, 0.3573572, tolerance=1e-6))
})

test_that("gets some random numbers between 0 and 1", {
  cModel <- .C_newSpeciesTable(0)
  expect_true(.C_initRNGState(cModel, 1, 100))  
  rndNumbers <- matrix(0, 10, 10)
  rndNumbers <- .C_getRandomNumbers(cModel, rndNumbers)
  for (i in 1:10) {
    for (j in 1:10) {
      expect_gte(rndNumbers[[i, j]], 0.0)
      expect_lte(rndNumbers[[i, j]], 1.0)
    }
  }
})