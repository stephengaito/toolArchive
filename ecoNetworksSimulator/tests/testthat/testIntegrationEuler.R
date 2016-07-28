context("Test integration")

test_that("we can build a C-model from an R-model", {
  r2c1 <- newTrophicModel()
  r2c1 <- addSpecies(r2c1, "resource1", growthRate = 0.2, carryingCapacity = 25, timeLag = 1,
                     reintroductionProb = 0.8, reintroductionSize=0.9)
  r2c1 <- addSpecies(r2c1, "resource2", growthRate = 0.3, carryingCapacity = 26, timeLag = 2,
                     reintroductionProb = 0.8, reintroductionSize=0.9)
  r2c1 <- addSpecies(r2c1, "consumer", mortality = 0.4, halfSaturation = 27,
                     reintroductionProb = 0.3, reintroductionSize=0.4)
  r2c1 <- addInteraction(r2c1,
                          predator = "consumer",
                          prey = "resource1",
                          attackRate = 0.5,
                          conversionRate = 0.35,
                          timeLag = 3)
  r2c1 <- addInteraction(r2c1,
                          predator = "consumer",
                          prey = "resource2",
                          attackRate = 0.6,
                          conversionRate = 0.40,
                          timeLag = 4)
  expect_equal(maximumTimeLag(r2c1), 4)
  cModel <- .R_buildCModel(r2c1, 0.01)
  numSpecies <- .C_numSpecies(cModel)
  expect_equal(numSpecies, 3)
  #
  # resource1
  #
  values <- .C_getSpeciesValues(cModel, 0)
  expect_equal(length(values), 7)
  expect_equal(values[1], 0.2)
  expect_equal(values[2], 25.0)
  expect_equal(values[3], 100.0)
  expect_equal(values[4], 0.0)
  expect_equal(values[5], NA_real_)
  expect_equal(values[6], 0.8)
  expect_equal(values[7], 0.9)
  preyValues <- .C_getPreyCoefficients(cModel, 0)
  expect_equal(length(preyValues), 0)
  predatorValues <- .C_getPredatorCoefficients(cModel, 0)
  expect_equal(length(predatorValues), 4)
  expect_equal(length(predatorValues[[1]]), 1)
  expect_equal(predatorValues[[1]][1], 2)
  expect_equal(length(predatorValues[[2]]), 1)
  expect_equal(predatorValues[[2]][1], 0.5)
  expect_equal(length(predatorValues[[3]]), 1)
  expect_equal(predatorValues[[3]][1], 0.35)
  expect_equal(length(predatorValues[[4]]), 1)
  expect_equal(predatorValues[[4]][1], 300)
  #
  # resource2
  #
  values <- .C_getSpeciesValues(cModel, 1)
  expect_equal(length(values), 7)
  expect_equal(values[1], 0.3)
  expect_equal(values[2], 26.0)
  expect_equal(values[3], 200.0)
  expect_equal(values[4], 0.0)
  expect_equal(values[5], NA_real_)
  expect_equal(values[6], 0.8)
  expect_equal(values[7], 0.9)
  preyValues <- .C_getPreyCoefficients(cModel, 1)
  expect_equal(length(preyValues), 0)
  predatorValues <- .C_getPredatorCoefficients(cModel, 1)
  expect_equal(length(predatorValues), 4)
  expect_equal(length(predatorValues[[1]]), 1)
  expect_equal(predatorValues[[1]][1], 2)
  expect_equal(length(predatorValues[[2]]), 1)
  expect_equal(predatorValues[[2]][1], 0.6)
  expect_equal(length(predatorValues[[3]]), 1)
  expect_equal(predatorValues[[3]][1], 0.40)
  expect_equal(length(predatorValues[[4]]), 1)
  expect_equal(predatorValues[[4]][1], 400)
  #
  # consumer
  #
  values <- .C_getSpeciesValues(cModel, 2)
  expect_equal(length(values), 7)
  expect_equal(values[1], 0)
  expect_equal(values[2], NA_real_)
  expect_equal(values[3], 0.0)
  expect_equal(values[4], 0.4)
  expect_equal(values[5], 27.0)
  expect_equal(values[6], 0.3)
  expect_equal(values[7], 0.4)
  preyValues <- .C_getPreyCoefficients(cModel, 2)
  expect_equal(length(preyValues), 4)
  expect_equal(length(preyValues[[1]]), 2)
  expect_equal(preyValues[[1]][1], 0)
  expect_equal(preyValues[[1]][2], 1)
  expect_equal(length(preyValues[[2]]), 2)
  expect_equal(preyValues[[2]][1], 0.5)
  expect_equal(preyValues[[2]][2], 0.6)
  expect_equal(length(preyValues[[3]]), 2)
  expect_equal(preyValues[[3]][1], 0.35)
  expect_equal(preyValues[[3]][2], 0.40)
  expect_equal(length(preyValues[[4]]), 2)
  expect_equal(preyValues[[4]][1], 300)
  expect_equal(preyValues[[4]][2], 400)
  predatorValues <- .C_getPredatorCoefficients(cModel, 2)
  expect_equal(length(predatorValues), 0)
})

test_that("we can euler integrate simple model", {
  r1c1 <- newTrophicModel()
  r1c1 <- addSpecies(r1c1, "resource", growthRate = 0.2, carryingCapacity = 25, timeLag = 15)
  r1c1 <- addSpecies(r1c1, "consumer", mortality = 0.2, halfSaturation = 25)
  r1c1 <- addInteraction(r1c1,
    predator = "consumer",
    prey = "resource",
    attackRate = 0.1,
    conversionRate = 0.35,
    timeLag = 0.3)
  #print(r1c1)
  numSpecies <- numSpeciesInModel(r1c1)
  initialValues <- matrix( 0.0, nrow = 1, ncol=numSpecies)
  #print(initialValues)
  results <- integrateModel(r1c1, 0.01, 10, 10, initialValues)
  #print(results)
  expect_false(is.null(results))
  expect_equal(results[1,1], 0.0)
  expect_equal(results[1,2], 0.0)
  expect_equal(results[10,1], 0.0)
  expect_equal(results[10,2], 0.0)
})