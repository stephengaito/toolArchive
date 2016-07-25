context("Models")

test_that("newTrophicModel creates a trophic model", {
  test <- newTrophicModel()
  expect_true(isTrophicModel(test))
  expect_error(isNormalTrophicModel(test))
})

test_that("addSpecies adds a species to a speciesTable", {
  model <- newTrophicModel()
  model <- addSpecies(model, "resource", growthRate=0.1, carryingCapacity=0.2, timeLag = 0.3, reintroductionRate = 0.4)
  model <- addSpecies(model, "consumer", mortality=0.5, halfSaturation=0.6, reintroductionRate = 0.7)
  species <- model$species
  speciesLevels <- levels(species$species)
  expect_true(isTrophicModel(model))
  expect_true(.R_isSpeciesTable(species))
  
  expect_equal(speciesLevels[species[1, "species"]], "resource")
  expect_equal(species[1, "growthRate"], 0.1)
  expect_equal(species[1, "carryingCapacity"], 0.2)
  expect_equal(species[1, "timeLag"], 0.3)
  expect_equal(species[1, "mortality"], 0.0)
  expect_equal(species[1, "halfSaturation"], NA_real_)
  expect_equal(species[1, "reintroductionRate"], 0.4)
  expect_equal(speciesLevels[species[2, "species"]], "consumer")
  expect_equal(species[2, "growthRate"], 0.0)
  expect_equal(species[2, "carryingCapacity"], NA_real_)
  expect_equal(species[2, "timeLag"], 0.0)
  expect_equal(species[2, "mortality"], 0.5)
  expect_equal(species[2, "halfSaturation"], 0.6)
  expect_equal(species[2, "reintroductionRate"], 0.7)
})

test_that("addInteraction adds an interaction between two species", {
  model <- newTrophicModel()
  model <- addInteraction(model, "consumer", "resource", attackRate = 0.1, conversionRate = 0.2, timeLag = 0.3)
  interactions <- model$interactions
  expect_true(.R_isInteractionsTable(interactions))
  predatorLevels <- levels(interactions$predator)
  preyLevels <- levels(interactions$prey)
  expect_equal(predatorLevels[interactions[1, "predator"]], "consumer")
  expect_equal(preyLevels[interactions[1, "prey"]], "resource")
  expect_equal(interactions[1, "attackRate"], 0.1)
  expect_equal(interactions[1, "conversionRate"], 0.2)
  expect_equal(interactions[1, "timeLag"], 0.3)
})

test_that(".R_areRelated correctly identifies if two tables are related", {
  model <- newTrophicModel()
  model <- addSpecies(model, "resource", growthRate=0.1, carryingCapacity=0.2, timeLag = 0.3)
  model <- addSpecies(model, "consumer", mortality=0.4)
  
  model <- addInteraction(model, "consumer", "resource", attackRate = 0.1, conversionRate = 0.2, timeLag = 0.3)
  expect_true(.R_areRelated(model$species, model$interactions))
  
  model <- addInteraction(model, "unknownConsumer", "unknownResource", 
                                 attackRate = 0.1, conversionRate = 0.2, timeLag = 0.3)
  expect_false(.R_areRelated(model$species, model$interactions))
})

test_that(".R_varyParameter returns a positive random number OR echos strings", {
  expect_equal(.R_varyParameter("resource","anOtherString"), "resource")
  expect_lte(0.0, .R_varyParameter(0.1, 0.1))
  expect_equal(0.0, .R_varyParameter(-100, 1))
})

test_that("varyModel works", {
  model <- newTrophicModel()
  model <- addSpecies(   model, "resource", growthRate=0.1, carryingCapacity=0.2, timeLag = 0.3)
  model <- addSpeciesStd(model, "resource", growthRate=0.01, carryingCapacity=0.02, timeLag = 0.03)
  model <- addSpecies(model, "consumer", mortality=0.4)
  model <- addSpeciesStd(model, "consumer", mortality=0.04)

  model <- addInteraction(   model, "consumer", "resource", attackRate = 0.1, conversionRate = 0.2, timeLag = 0.3)
  model <- addInteractionStd(model, "consumer", "resource", attackRate = 0.01, conversionRate = 0.02, timeLag = 0.03)
  
  expect_equal(model$species[1, "growthRate"], 0.1)
  expect_equal(model$speciesStd[1, "growthRate"], 0.01)
  expect_equal(model$interactions[1, "attackRate"], 0.1)
  expect_equal(model$interactionStds[1, "attackRate"], 0.01)
  aModel <- varyModel(model)
  expect_equal(nrow(aModel$species), 2)
  expect_equal(nrow(aModel$interactions), 1)
})