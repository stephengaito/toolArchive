context("Test the model bulding interface between R and C")

test_that("C_newSpeciesTable creates a species table in C", {
  test <- .C_newSpeciesTable(10)
  expect_false(is.null(test))
  expect_true(.C_isSpeciesTable(test))
})

test_that("C_numSpecies reports the correct number of species", {
  test <- .C_newSpeciesTable(10)
  expect_equal(.C_numSpecies(test), 10)
})

test_that("C_getSpeciesValues gets the correct values", {
  test <- .C_newSpeciesTable(10)
  expect_false(.C_setSpeciesValues(test, 50, c(0.1, 0.2, 0.3)))
  expect_true(.C_setSpeciesValues(test, 5, c(0.1, 0.2, 0.3)))
  expect_true(.C_setSpeciesValues(test, 6, c(0.1, NA_real_, 0.3)))
  values <- .C_getSpeciesValues(test, 2)
  expect_equal(length(values), 3)
  expect_equal(values[1], NA_real_)
  expect_equal(values[2], NA_real_)
  expect_equal(values[3], NA_real_)
  values <- .C_getSpeciesValues(test, 5)
  expect_equal(length(values), 3)
  expect_equal(values[1], 0.1)
  expect_equal(values[2], 0.2)
  expect_equal(values[3], 0.3)
  values <- .C_getSpeciesValues(test, 6)
  expect_equal(length(values), 3)
  expect_equal(values[1], 0.1)
  expect_equal(values[2], NA_real_)
  expect_equal(values[3], 0.3)
})

test_that("C_newInteractionsTable creates an interaction table in C", {
  test <- .C_newInteractionsTable(10)
  expect_false(is.null(test))
  expect_true(.C_isInteractionsTable(test))
})