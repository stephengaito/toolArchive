context("Test integration of Lorenz equations")

test_that("we can euler integrate the Lorenz equations", {
  results <- integrateLorenz(numSamples = 10)
  expect_false(is.null(results))
  expect_equal(nrow(results), 10)
  expect_equal(ncol(results), 3)
  expect_false(is.na(results[10, 1]))
})