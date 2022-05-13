#!/usr/bin/env Rscript

#TL;DR
#R SUCKS AT SAVING PLOTS TO A FILE
#it messes up the dimensions every time wtf

args <- commandArgs(trailingOnly=TRUE)
if(length(args) != 1) {
	print(args)
	stop("Usage: Rscript pixel_density.r filename")
}
setup <- function() {
  library(jpeg)
  library(ggplot2)
  library(gridExtra)
  library(tidyverse)
  setwd("~/classroom/csci551/final_project")
}
setup()
filename <- args[1]
img <- readJPEG(filename)
dims <- attributes(img)$dim
if(dims[3] != 3) {
  print("This could be more flexible. I'll do it later")
  stop("Channels not RGB")
}
rgb <- data.frame(red=as.vector(img[,,1]),
                  green=as.vector(img[,,2]),
                  blue=as.vector(img[,,3]))
rgb <- rgb %>% 
  pivot_longer(cols=c("red", "green", "blue"), names_to="color")

densities <- ggplot(data=rgb, aes(x=value, group=color, fill=color)) + 
	geom_density() + ggtitle("Original Pixel Density") 

ggsave('ggsave.png', device='png', height=837, width=437, units='mm')
ggsave('ggsave.jpeg', device='jpeg')
ggsave('ggsave.pdf', device='pdf')

out_file <- 'og_densities.pdf'
#pdf(out_file, width=par('din')[1], height=par('din')[2])
pdf(out_file, width=837, height=431)
ggplot(data=rgb, aes(x=value, group=color, fill=color)) + 
	geom_density() + 
	scale_fill_manual(values=c("red", "green", "blue")) +
	ggtitle("Original Pixel Density")
dev.off()

#system(command=paste("open", out_file)) 
