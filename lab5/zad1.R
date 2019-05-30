library("ggplot2")
  
#load data from file
input = read.csv("data.csv")

#initialise dataframes naive,blas
formatted_data= aggregate(cbind(naive,blas) ~size,data=input,FUN=mean)

naive=data.frame(x=formatted_data$size,y=formatted_data$naive)
naive$sd = aggregate(naive ~size,data=input,FUN=sd)$naive
blas=data.frame(x=formatted_data$size,y=formatted_data$blas)
blas$sd = aggregate(blas ~size,data=input,FUN=sd)$blas


dense_points = data.frame(x = seq(0,700, length.out=800))
naive_fit = lm(y~ poly(x,3,raw=TRUE), data = data.frame(naive["x"],naive["y"]))
dense_points$naive_y = predict(naive_fit,dense_points)
blas_fit = lm(y~ poly(x,3,raw=TRUE), data = data.frame(blas["x"],blas["y"]))
dense_points$blas_y = predict(blas_fit,dense_points)


#initialise plot 
ggplot(naive,aes(x,y))
last_plot() + geom_point(  ) + geom_point(data=blas,colour='blue')    
last_plot() + geom_errorbar(aes(ymin=naive$y-naive$sd,ymax=naive$y+naive$sd),width=1,colour="red")
last_plot() + geom_errorbar(aes(ymin=blas$y-blas$sd,ymax=blas$y+blas$sd),width=1,colour="blue")
last_plot() + geom_line(data=dense_points,aes(x,naive_y),colour="red")
last_plot() + geom_line(data=dense_points,aes(x,blas_y),colour="blue")
last_plot() + labs(title="naive and blas matrix speed comparison",y="time",x="matrix size")
