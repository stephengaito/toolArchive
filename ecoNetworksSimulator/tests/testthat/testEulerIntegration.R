context("Test integration")

test_that("we can build a C-model from an R-model", {
  species <- newSpeciesTable()
  species <- addSpecies(species, "resource1", growthRate = 0.2, carryingCapacity = 25)
  species <- addSpecies(species, "resource2", growthRate = 0.2, carryingCapacity = 25)
  species <- addSpecies(species, "consumer", mortality = 0.2, halfSaturation = 25)
  interactions <- newInteractionsTable()
  interactions <- addInteraction(interactions,
                                 predator = "consumer",
                                 prey = "resource1",
                                 attackRate = 0.1,
                                 conversionRate = 0.35)
  interactions <- addInteraction(interactions,
                                 predator = "consumer",
                                 prey = "resource2",
                                 attackRate = 0.1,
                                 conversionRate = 0.35)
  r2c1 <- newModel(species, interactions)
  cModel <- .R_buildCModel(r2c1)
  numSpecies <- .C_numSpecies(cModel)
  expect_equal(numSpecies, 3)
  for( species in 1:numSpecies ) {
    values <- .C_getSpeciesValues(cModel, species-1)
    print(values)
    predatorValues <- .C_getPredatorCoefficients(cModel, species-1)
    print(predatorValues)
    preyValues <- .C_getPreyCoefficients(cModel, species-1)
    print(preyValues)
  }
})

test_that("we can euler integrate simple model", {
  species <- newSpeciesTable()
  species <- addSpecies(species, "resource1", growthRate = 0.2, carryingCapacity = 25)
  species <- addSpecies(species, "resource2", growthRate = 0.2, carryingCapacity = 25)
  species <- addSpecies(species, "consumer", mortality = 0.2, halfSaturation = 25)
  interactions <- newInteractionsTable()
  interactions <- addInteraction(interactions,
    predator = "consumer",
    prey = "resource1",
    attackRate = 0.1,
    conversionRate = 0.35)
  interactions <- addInteraction(interactions,
    predator = "consumer",
    prey = "resource2",
    attackRate = 0.1,
    conversionRate = 0.35)
  r2c1 <- newModel(species, interactions)
  values <- integrateModel(r2c1)
  expect_false(is.null(values))
})