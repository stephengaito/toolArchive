context("Test the model bulding interface between R and C")

test_that("C_newSpeciesTable creates a species table in C", {
  test <- .Call("C_newSpeciesTable", as.integer(10), PACKAGE = "ecoNetworksSimulator")
  expect_false(is.null(test))
  expect_true(.Call("C_isSpeciesTable", test, PACKAGE = "ecoNetworksSimulator"))
})