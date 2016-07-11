context("Models")

test_that("create some models", {
  createModel("test0")
  models <- listModels()
  expect_equal(length(models), 1)
})