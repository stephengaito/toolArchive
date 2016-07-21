context("Models")

test_that("newSpeciesTable creates a species table", {
  test <- newSpeciesTable()
  expect_true(isSpeciesTable(test))
})

test_that("addSpecies adds a species to a speciesTable", {
  test <- newSpeciesTable()
  test <- addSpecies(test, "resource", growthRate=0.1, carryingCapacity=0.2, timeLag = 0.3)
  test <- addSpecies(test, "consumer", mortality=0.4, halfSaturation=0.5)
  speciesLevels <- levels(test$species)
  expect_true(isSpeciesTable(test))
  expect_equal(speciesLevels[test[1, "species"]], "resource")
  expect_equal(test[1, "growthRate"], 0.1)
  expect_equal(test[1, "carryingCapacity"], 0.2)
  expect_equal(test[1, "timeLag"], 0.3)
  expect_equal(test[1, "mortality"], 0.0)
  expect_equal(test[1, "halfSaturation"], NA_real_)
  expect_equal(speciesLevels[test[2, "species"]], "consumer")
  expect_equal(test[2, "growthRate"], 0.0)
  expect_equal(test[2, "carryingCapacity"], NA_real_)
  expect_equal(test[2, "timeLag"], 0.0)
  expect_equal(test[2, "mortality"], 0.4)
  expect_equal(test[2, "halfSaturation"], 0.5)
})

test_that("newInteractionsTable creates an interactions table", {
  test <- newInteractionsTable()
  expect_true(isInteractionsTable(test))
})

test_that("addInteraction adds an interaction between two species", {
  test <- newInteractionsTable()
  test <- addInteraction(test, "consumer", "resource", attackRate = 0.1, conversionRate = 0.2, timeLag = 0.3)
  expect_true(isInteractionsTable(test))
  predatorLevels <- levels(test$predator)
  preyLevels <- levels(test$prey)
  expect_equal(predatorLevels[test[1, "predator"]], "consumer")
  expect_equal(preyLevels[test[1, "prey"]], "resource")
  expect_equal(test[1, "attackRate"], 0.1)
  expect_equal(test[1, "conversionRate"], 0.2)
  expect_equal(test[1, "timeLag"], 0.3)
})

test_that("areRelated correctly identifies if two tables are related", {
  speciesTable <- newSpeciesTable()
  speciesTable <- addSpecies(speciesTable, "resource", growthRate=0.1, carryingCapacity=0.2, timeLag = 0.3)
  speciesTable <- addSpecies(speciesTable, "consumer", mortality=0.4)
  
  interactionsTable <- newInteractionsTable()
  interactionsTable <- addInteraction(interactionsTable, 
                                      "consumer", "resource", attackRate = 0.1, conversionRate = 0.2, timeLag = 0.3)
  expect_true(areRelated(speciesTable, interactionsTable))
  
  interactionsTable <- addInteraction(interactionsTable, 
                                      "unknownConsumer", "unknownResource", 
                                      attackRate = 0.1, conversionRate = 0.2, timeLag = 0.3)
  expect_false(areRelated(speciesTable, interactionsTable))
})

test_that("varyParameter returns a positive random number OR echos strings", {
  expect_equal(varyParameter("resource","anOtherString"), "resource")
  expect_lte(0.0, varyParameter(0.1, 0.1))
  expect_equal(0.0, varyParameter(-100, 1))
})

test_that("varyModel works", {
  speciesTable <- newSpeciesTable()
  speciesTable <- addSpecies(speciesTable, "resource", growthRate=0.1, carryingCapacity=0.2, timeLag = 0.3)
  speciesTable <- addSpecies(speciesTable, "consumer", mortality=0.4)
  speciesStd <- newSpeciesTable()
  speciesStd <- addSpecies(speciesStd, "resource", growthRate=0.01, carryingCapacity=0.02, timeLag = 0.03)
  speciesStd <- addSpecies(speciesStd, "consumer", mortality=0.04)

  interactionsTable <- newInteractionsTable()
  interactionsTable <- addInteraction(interactionsTable, 
                                      "consumer", "resource", attackRate = 0.1, conversionRate = 0.2, timeLag = 0.3)
  interactionsStd <- newInteractionsTable()
  interactionsStd <- addInteraction(interactionsStd, 
                                      "consumer", "resource", attackRate = 0.01, conversionRate = 0.02, timeLag = 0.03)
  
  normalModel <- newNormalModel(speciesTable, speciesStd, interactionsTable, interactionsStd)
  expect_equal(normalModel$species[1, "growthRate"], 0.1)
  expect_equal(normalModel$speciesStd[1, "growthRate"], 0.01)
  expect_equal(normalModel$interactions[1, "attackRate"], 0.1)
  expect_equal(normalModel$interactionStds[1, "attackRate"], 0.01)
  aModel <- varyModel(normalModel)
  expect_equal(nrow(aModel$species), 2)
  expect_equal(nrow(aModel$interactions), 1)

})