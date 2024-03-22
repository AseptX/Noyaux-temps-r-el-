# Noyaux-temps-r-el-

Le vTaskDelay(100) prend 100 tick du timer 6 mais un tick ne vaut pas forcement 1 ms donc on aura pas forcement 100 ms de wait 

Grace au maccro portTICK_PERIOD_MS, on règle le fait qu'un tick vale 1 ms (ou autres valeur mais ici c'est 1 ms qui nous interesse) 
