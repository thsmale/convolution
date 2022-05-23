path <- "images/valley_oak/"
files <- list.files(path)
for (file in files) {
	file <- paste(path, file, sep = "")
	command <- paste("./run.sh", file)
	system(command)
}
