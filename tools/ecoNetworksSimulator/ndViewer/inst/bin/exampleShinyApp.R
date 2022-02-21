#!/usr/bin/env Rscript

# This Rscript loads and runs a shiny app to display the a simple 4-d cube

library(shiny)
library(dynSysToolKit)
library(plot3D)
library(ndViewer)

coordLabels <- c("x", "y", "z")

server <- function(input, output) {
  
  results <- reactive({
    integrateLorenz(
      x = as.double(input$x),
      y = as.double(input$y),
      z = as.double(input$z),
      sigma = as.double(input$sigma),
      rho = as.double(input$rho),
      beta = as.double(input$beta),
      stepsPerSample = 1,
      numSamples = 1000)
  })
  
  output$echo <- renderUI(
    inputPanel(
      p("x = ", input$x),
      p("y = ", input$y),
      p("z = ", input$z),
      hr(),
      p("sigma = ", as.double(input$sigma)),
      p("rho   = ", as.double(input$rho)),
      p("beta  = ", as.double(input$beta))
    )
  )
  
  output$results <- renderTable(results())
  
  output$distPlot <- renderPlot({
    lines3D(results()[,1], results()[,2], results()[,3], col="blue")
  })
}

ui <- fluidPage(
  verticalLayout(
    tabsetPanel(
      tabPanel(
        "parameters",
        inputPanel(
          p("Lorenz parameters"),
          numericInput("sigma", "sigma", 10.0, step = 0.1),
          numericInput("rho",   "rho",   28.0, step = 0.1),
          numericInput("beta",  "beta",  (8.0/3.0), step = 0.1)
        )
      ),
      tabPanel(
        "initial conditions",
        inputPanel(
          p("Lorenz initial conditions"),
          numericInput("x", "x", 0.1),
          numericInput("y", "y", 0.1),
          numericInput("z", "z", 0.1)
        )
      ),
      tabPanel(
        "view point",
        viewPointUI(coordLabels)
      ),
      tabPanel(
        "view angle",
        viewAngleUI(coordLabels)
      )
    ),
    plotOutput("distPlot"),
    htmlOutput("echo")
    #tableOutput("results")
  )
)

shinyApp(ui = ui, server = server)
