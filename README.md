# Noyaux-temps-r-el-

Le vTaskDelay(100) prend 100 tick du timer 6 mais un tick ne vaut pas forcement 1 ms donc on aura pas forcement 100 ms de wait 

Grace au maccro portTICK_PERIOD_MS, on règle le fait qu'un tick vale 1 ms (ou autres valeur mais ici c'est 1 ms qui nous interesse) 


Semaphore binaire : Permet la synchronisation de deux taches (une qui se lance et qui reveille l'autre ensuite)
Semaphore mutex (mutual exclusion) : Permet de finir une tache avant d'en lancer une autre meme si elle avait du se lancer

Exemple : Tache 2 + prioritaire que tache 1, elle se lance au plein milieu de la tache 1. Avec un mutex, elle va se lancer et bloquer au take qui s'execute avant le printf
puis on va retourner dans la tache 1 jusqu'au give qui va reveiller la tache 2. Du coup on a les printf qui s'execute l'un après l'autre et on a plus le problème d'écriture


Si on utilise la fonction HAL_UART_Receive pour traiter les caractères, c'est une boucle infini => on bloque le micro jusqu'a qu'on recoive un char. 

On utilise la fonction HAL_UART_Receive_IT (dans la fonction uart_read dans le fichier shell.C) qui active les interruptions sur caractere et on utilise un semaphore take pour bloquer la tache et qui est give par la fonction shell_uart_receive_irq_cb qui est dans le fichier shell.c
Cette fonction est appelé par la fonction HAL_UART_RxCpltCallback qui est dans le fichier main et appelée lors d'une interruption de réception de caractère

L'allocation dynamique se fait dans le tas. Il y en a 2, un geré par la HAL et un geré par freeRTOS, nous on utilise celui géré par freeRTOS. 
Sa taille est comprise dans l'utilisation de la RAM qu'on voit en bas à droite, si on augmente sa taille, on voit que la taille de la RAM utilisé augmente. 

Avec la taille de base on utilise environ 5% de la RAM et le tas est plein au bout de la 12e tache généré. Si on augmente la taille du tas par 10, on est environ à 50% et on peut generer 127 taches 


![image](https://github.com/AseptX/Noyaux-temps-r-el-/assets/144770585/d4bd84f3-75fe-49a8-b9cf-8988fa8ba854)

3.3) On créer un timer qui doit compter entre 10 et 100 fois plus vite que le tick de l'OS (1ms). Le but est de compter le temps qu'on passe dans chaque tache
Compter trop doucement peut nous faire perdre en précision si la tache est trop rapide et compter trop vite fait qu'on arrivera à la valeur max du timer plus vite
Le timer a une horloge de 108  MHz, on met son prescaler à 10800 pour compter à 10 kHz. On écrit les deux fonctions donné dans le TP comme ceci : 

//Active le timer 2
void configureTimerForRunTimeStats(void)
{
	HAL_TIM_Base_Start(&htim2);
}

 //Renvois la valeur du timer
unsigned long getRunTimeCounterValue(void)
{
	return __HAL_TIM_GET_COUNTER(&htim2);
}

Le calcul du temps passé dans les taches se fait automatiquement et on trouve  ca : 

![image](https://github.com/AseptX/Noyaux-temps-r-el-/assets/144770585/d845ed8f-79b4-4928-9d64-ced8710dfee7)



