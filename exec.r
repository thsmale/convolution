files <- list.files("imgaes/valley_oak/")
for (file in files) {
	command <- paste("./run.sh", file)
	system(command)
}
