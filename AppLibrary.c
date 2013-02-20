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
		rand=F_JKISS();
		pos.j=rand*100.0;
		Agente = newAgent(pos,r,t,GrowthRate);
		Event = newAppendEvent(Agente,NULL,t);
		InsertAgentInField(Agente,&Field);
		Insert_Event_In_Queue(Event, &Queue);		
	}
	Queue.first=Min_Element(Queue.root);
	
	float Time=0.0;
	while(Time<FinalTime){
		Time=Process_Queue(&Queue);
	}

	FastForward(Field,Time);

	WriteStateToFile(Field,fileNameToStore,Time);
	
	FreeSystem(&Field);

return;
}

void SimpleDeterministicMovie(float FinalTime,char *fileNameToStore)
{
	int NoIndividuos = 2;
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
	
	sitio pos;
	
	int i;
	for(i=0;i<NoIndividuos;i++)
	{	
		pos.i=(i+1)*10.0;
		pos.j=10.0;
		Agente = newAgent(pos,r,t,GrowthRate);
		Event = newAppendEvent(Agente,NULL,t);
		InsertAgentInField(Agente,&Field);
		Insert_Event_In_Queue(Event, &Queue);		
	}
	Queue.first=Min_Element(Queue.root);

	float Time=0.0;
	float RuleTime=0.0;
	float DT=1.0;
	while(Time<FinalTime){
		if(RuleTime < Queue.first->data->time)
		{
			FastForward(Field,RuleTime);
			Time=RuleTime;
			WriteStateToFile(Field,fileNameToStore,RuleTime);
			RuleTime+=DT;
		}else{
			Time=Process_Queue(&Queue);
		}
	}
	
	FreeSystem(&Field);

return;
}



main(){
	float Time = 20.0;
	SimpleDeterministicMovie(Time,"DATOS/fix/b");
	
return;
}
