context("Initial tests")

test_that("simulateModel echos argument", {
  expect_equal(simulateModel(pi), pi)
})