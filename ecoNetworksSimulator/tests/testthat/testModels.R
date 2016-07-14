context("Models")

test_that("newSpeciesTable creates a species table", {
  test <- newSpeciesTable()
  expect_true(isSpeciesTable(test))
})

test_that("addSpecies adds a species to a speciesTable", {
  test <- newSpeciesTable()
  test <- addSpecies(test, "resource", growthRate=0.1, carryingCapacity=0.2)
  test <- addSpecies(test, "consumer", mortality=0.3, halfSaturation=0.5)
  speciesLevels <- levels(test$species)
  expect_true(isSpeciesTable(test))
  expect_equal(speciesLevels[test[1, "species"]], "resource")
  expect_equal(test[1, "growthRate"], 0.1)
  expect_equal(test[1, "carryingCapacity"], 0.2)
  expect_equal(test[1, "mortality"], NA_real_)
  expect_equal(test[1, "halfSaturation"], NA_real_)
  expect_equal(speciesLevels[test[2, "species"]], "consumer")
  expect_equal(test[2, "growthRate"], 0.0)
  expect_equal(test[2, "carryingCapacity"], NA_real_)
  expect_equal(test[2, "mortality"], 0.3)
  expect_equal(test[2, "halfSaturation"], 0.5)
})

test_that("newInteractionsTable creates an interactions table", {
  test <- newInteractionsTable()
  expect_true(isInteractionsTable(test))
})

test_that("addInteraction adds an interaction between two species", {
  test <- newInteractionsTable()
  test <- addInteraction(test, "consumer", "resource", attackRate = 0.1, conversionRate = 0.2)
  expect_true(isInteractionsTable(test))
  predatorLevels <- levels(test$predator)
  preyLevels <- levels(test$prey)
  expect_equal(predatorLevels[test[1, "predator"]], "consumer")
  expect_equal(preyLevels[test[1, "prey"]], "resource")
  expect_equal(test[1, "attackRate"], 0.1)
  expect_equal(test[1, "conversionRate"], 0.2)
})

test_that("areRelated correctly identifies if two tables are related", {
  speciesTable <- newSpeciesTable()
  speciesTable <- addSpecies(speciesTable, "resource", growthRate=0.1, carryingCapacity=0.2)
  speciesTable <- addSpecies(speciesTable, "consumer", mortality=0.3)
  
  interactionsTable <- newInteractionsTable()
  interactionsTable <- addInteraction(interactionsTable, "consumer", "resource", attackRate = 0.1, conversionRate = 0.2)
  expect_true(areRelated(speciesTable, interactionsTable))
  
  interactionsTable <- addInteraction(interactionsTable, "unknownConsumer", "unknownResource", attackRate = 0.1, conversionRate = 0.2)
  expect_false(areRelated(speciesTable, interactionsTable))
})