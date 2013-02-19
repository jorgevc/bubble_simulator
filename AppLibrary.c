#include "collisionLib.h"
#include "GNA.h"
#include <stddef.h>
#include <stdio.h>

void EvolveSystem(float FinalTime,char *fileNameToStore)
{
	int NoIndividuos = 50;
	float r = 0.0;
	float t=0.0;
	int GrowthRate = 1;
	agentList Field;
	data_tree Queue;

	Field.NoMembers=0;
	Field.members=NULL;
	Queue.root=NULL;
	agent *Agente;
	event *Event;
	
	//init_JKISS();
	float rand;
	sitio pos;
	
	init();
	int i;
	for(i=0;i<NoIndividuos;i++)
	{
		rand=F_JKISS();
		pos.i=rand*100.0;
		pos.j=rand*100.0;
		Agente = newAgent(pos,r,t,GrowthRate);
		Event = newAppendEvent(Agente,NULL,t);
		InsertAgentInField(Agente,&Field);
		Insert_Event_In_Queue(Event, &Queue);
		
	}

	float Time=0.0;
	while(Time<FinalTime){
		Time=Process_Queue(&Queue);
	}

	FastForward(Field,Time);

	WriteStateToFile(Field,fileNameToStore,Time);
	
	FreeSystem(&Field);

return;
}

main(){
	float Time = 100.0;
	EvolveSystem(Time,"DATOS3/b/billar.dat");
	
return;
}
