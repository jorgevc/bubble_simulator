/*
Copyright 2012 Jorge Velazquez
*/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <fftw3.h>
#include <stddef.h>
#include <sys/stat.h>
#include <string.h>
#include "GNA.h"
#include "collisionLib.h"

node *nil;


void init()
{
	nil=(node *)malloc(sizeof(node));
	nil->data=NULL;
	nil->before=nil;
	nil->after=nil;
	nil->parent=nil;
return;
}
//// Tree list managment
node* newNode(event *new)
{
	node *capsule;
	capsule=(node *)malloc(sizeof(node));
	capsule->data=new;
	new->Node=capsule;
	capsule->before=NULL;
	capsule->after=NULL;
	capsule->parent=NULL;
	
	return capsule;
}



node* Dettach_Node(node *to_detach,node *root)
{
	if(to_detach==root)
	{
		root=Attacht_Node(to_detach->after,to_detach->before);
		root->parent=NULL;
		to_detach->after = NULL;
		to_detach->before = NULL;
	}else{
		if(to_detach->parent == NULL)
		{
			return root;
		}else{ //No checo si es del mismo arbol o no, se asume que estan en mismo arbol.
			if(to_detach->parent->before == to_detach)
			{		
				to_detach->parent->before=Attacht_Node(to_detach->after,to_detach->before);
				if(to_detach->parent->before != NULL)
				{
					to_detach->parent->before->parent=to_detach->parent;
				}
				to_detach->parent = NULL;
				to_detach->before = NULL;
				to_detach->after = NULL;
			}else{
				to_detach->parent->after=Attacht_Node(to_detach->before,to_detach->after);
				if(to_detach->parent->after != NULL)
				{
					to_detach->parent->after->parent=to_detach->parent;
				}
				to_detach->parent = NULL;
				to_detach->before = NULL;
				to_detach->after = NULL;
			}
		}
	}
return root;	
}

node* Attacht_Node(node *to_attacht,node *root)
{
	if(to_attacht==NULL)
	{
		return root;
	}
	
	if(root==NULL)
	{
		return to_attacht;
	}
		
	node *queue_pointer=root;
	
	while(1)
	{	
		if( queue_pointer->data->time < to_attacht->data->time )
		{
			if(queue_pointer->after == NULL)
			{
				queue_pointer->after = to_attacht;
				to_attacht->parent=queue_pointer;
				break;
			}else{
				queue_pointer=queue_pointer->after;
			}
		}else{
			if(queue_pointer->before == NULL)
			{
				queue_pointer->before = to_attacht;
				to_attacht->parent=queue_pointer;
				break;
			}else{
				queue_pointer=queue_pointer->before;
			}
		}
	}
	
	return root;
}

node* Min_Element(node *root)
{
	if(root == NULL)
	{
		return NULL;
	}
	node *queue_pointer=root;

	while(queue_pointer->before != NULL)
	{	
		queue_pointer=queue_pointer->before;		
	}
	
return queue_pointer;
}

//// Manage event
event* newAppendEvent(agent *Agent1,agent *Agent2, float t)
{
	event *new;

	new=(event *)malloc(sizeof(event));
	new->time=t;
	new->indv1=Agent1;
	new->indv2=Agent2;
	Agent1->NextInteraction=new;
	new->Node=NULL;
	
return new;
}

void Insert_Event_In_Queue(event *new, data_tree *queue)
{
	node *root=queue->root;
	if(new->time < INF)
	{
		node *new_root;
		new_root=Attacht_Node(newNode(new),root);
		queue->root=new_root;
	}
	
return;
}

void Process_Event(event *collision)
{	
	if(collision->indv2==NULL)
	{	
		return;
	}
		float r1,t1,t2;
		if(collision->indv1->GrowthRate > 0)
		{
			r1=collision->indv1->radius;	
			t1=collision->indv1->time;
			t2=collision->time;
			
			collision->indv1->radius=Radius(r1, t1, t2);
			collision->indv1->time=collision->time;
			collision->indv1->GrowthRate=0;
		}
		
		if(collision->indv2->GrowthRate > 0)
		{
			r1=collision->indv2->radius;	
			t1=collision->indv2->time;
			t2=collision->time;
			
			collision->indv2->radius=Radius(r1, t1, t2);
			collision->indv2->time=collision->time;
			collision->indv2->GrowthRate=0;
		}
return;
}

void Update_Next_Event(agent *indv)
{
	int i,partner;
	float tmin,t;
	agentList vecinos=indv->vecinos;
	
	if(vecinos.NoMembers>0)
	{
		tmin=event_time(indv,vecinos.members[0]);
		partner=0;
		for(i=1 ; i < vecinos.NoMembers ; i++)
		{
			t=event_time(indv,vecinos.members[i]);
				if( t < tmin)
				{
					tmin=t;
					partner=i;
				}
		}
		
		indv->NextInteraction->time=tmin;
		indv->NextInteraction->indv2=vecinos.members[partner];
	}else{
		indv->NextInteraction->time=INF;
		indv->NextInteraction->indv2=NULL;
	}
		
return;
}


float Process_Queue(data_tree *queue)
{	
event *processing=NULL;
node *root=queue->root;
event new_event;
float Time;
	
	
	if(root != NULL)
	{
		processing=(event *)malloc(sizeof(event));
		
		*processing=*(queue->first->data);	
		Process_Event(processing);
		Update_Next_Event(processing->indv1);
		root=Dettach_Node(processing->indv1->NextInteraction->Node,root);
		root=Attacht_Node(processing->indv1->NextInteraction->Node,root);	
		if(processing->indv1->NextInteraction->indv2!=NULL)
		{
			if(processing->indv1->NextInteraction->indv2->NextInteraction->indv2 == processing->indv1)
			{
				Update_Next_Event(processing->indv1->NextInteraction->indv2);
				root=Dettach_Node(processing->indv1->NextInteraction->indv2->NextInteraction->Node,root);
				root=Attacht_Node(processing->indv1->NextInteraction->indv2->NextInteraction->Node,root);	
			}
		}
		if(processing->indv2!=NULL)
		{
			Update_Next_Event(processing->indv2);
			root=Dettach_Node(processing->indv2->NextInteraction->Node,root);
			root=Attacht_Node(processing->indv2->NextInteraction->Node,root);
			if(processing->indv2->NextInteraction->indv2!=NULL)
			{
				if(processing->indv2->NextInteraction->indv2->NextInteraction->indv2 == processing->indv2)
				{
				Update_Next_Event(processing->indv2->NextInteraction->indv2);
				root=Dettach_Node(processing->indv2->NextInteraction->indv2->NextInteraction->Node,root);
				root=Attacht_Node(processing->indv2->NextInteraction->indv2->NextInteraction->Node,root);
				}
			}
		}
		Time=processing->time;
		free(processing);	
		queue->root=root;
		queue->first=Min_Element(root);
		return Time;
	}else{
		return INF;
	}

}
/////////////////// manage agent
agent* newAgent(sitio pos,float r, float t, int GrowthRate)
{
agent *new;

	new=(agent *)malloc(sizeof(agent));
	new->position=pos;
	new->radius=r;
	new->time=t;
	new->GrowthRate=GrowthRate;
	new->vecinos.NoMembers=0;
	new->vecinos.members=NULL;
	new->NextInteraction=NULL;

return new;
}



void InsertAgentInField(agent *AgentToInsert,agentList *field)
{
	////Inserto nuevo miembro a la lista
	newAppendAgentToList(AgentToInsert,field);
	// Veo quien es vecino y actualizo listas.
	agent *indv1=AgentToInsert;
	agent *indv2=NULL;
	float d2;
	int i;
	for(i=0;i<(field->NoMembers - 1);i++)
	{
		indv2=field->members[i];
		d2=(pow(indv1->position.i - indv2->position.i, 2.0) + pow(indv1->position.j - indv2->position.j, 2.0));
		if(d2<P4R_MAX2)
		{
			newAppendAgentToList(indv1,&(indv2->vecinos));
			newAppendAgentToList(indv2,&(indv1->vecinos));
		}
	}
return;
}

void newAppendAgentToList(agent *Agent, agentList *list)
{
	list->NoMembers++;
	int No=list->NoMembers;
	list->members=(agent **)realloc(list->members, No*sizeof(agent *));
	if ( list->members == NULL ) 
	{
				printf("Error en memoria de agentList!\n");
				exit(0);
	}
	list->members[No-1]=Agent;
return;
}

void FastForward(agentList list,float Time)
{
	float r1,t1;
	int i;
	for(i=0;i<list.NoMembers;i++)
	{
		if(list.members[i]->GrowthRate>0)
		{
			r1=list.members[i]->radius;
			t1=list.members[i]->time;
			if(t1>Time)
			{
				printf("O NO! individuo en el futuro!!!\n");
			}else{
				list.members[i]->radius=Radius(r1, t1,Time);
				list.members[i]->time=Time;
			}
		}
	}
return;
}

///////Auxiliaries
void WriteStateToFile(agentList list,char *fileName,float Time)
{
	char completeName[550];
	char copyFileName[550];
	char dir[545];
	FILE *file;
	char *pch = fileName;
	int d=0;
	
	strcpy(copyFileName,fileName);
	pch = strtok (copyFileName,"/");
	while(pch != NULL)
	{
		d++;
		 pch = strtok (NULL, "/");
	}
	
	if(d>1)
	{
		
		strcpy(copyFileName,fileName);
		int j=1;
		
		pch = strtok (copyFileName,"/");
		strcpy (dir,pch);
		 strcat (dir,"/");
		mkdir(dir,(S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH));
		  while (j < (d-1))
		  {
			  pch = strtok (NULL, "/");
			  strcat (dir,pch);
			  strcat (dir,"/");
			 mkdir(dir,(S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH));
			j++;
		  }	
	}
	
	sprintf(completeName,"%s_T_%3.2f",fileName,Time);
	file=fopen(completeName, "w");
	fputs("# x y radius\n",file);
	int i;
	for(i=0;i<list.NoMembers;i++)
	{
		fprintf(file,"%3.2f %3.2f %3.2f\n",list.members[i]->position.i, list.members[i]->position.j, list.members[i]->radius); 
	}
	fclose(file);
}

void FreeSystem(agentList *Field)
{
	int i;
	for(i=0;i<Field->NoMembers;i++)
	{
		free(Field->members[i]->NextInteraction->Node);
		free(Field->members[i]->NextInteraction);
		free(Field->members[i]->vecinos.members);
		free(Field->members[i]);
	}
	free(Field->members);
	Field->NoMembers=0;
return;
}
//////Grapers de modelo
float event_time(const agent *indv1,const agent *indv2)
{
	float r11, r12, t11, t12, t2, d2, T;
		r11=indv1->radius;
		r12=indv2->radius;
		t11=indv1->time;
		t12=indv2->time;
	if(indv1->GrowthRate > 0 && indv2->GrowthRate > 0)
	{	
		d2=(pow(indv1->position.i - indv2->position.i, 2.0) + pow(indv1->position.j - indv2->position.j, 2.0));
		T=ContactTime(r11,t11,r12,t12,d2);
	}else{
		if(indv1->GrowthRate > 0)
		{
			d2=(pow(indv1->position.i - indv2->position.i, 2.0) + pow(indv1->position.j - indv2->position.j, 2.0));
			T=ContacTime1(r11,t11,sqrt(d2)-r12);
		}else{
			if(indv2->GrowthRate > 0)
			{		
				d2=(pow(indv1->position.i - indv2->position.i, 2.0) + pow(indv1->position.j - indv2->position.j, 2.0));
				T=ContacTime1(r12,t12,sqrt(d2)-r11);
			}else{
				T=INF;
			}
		}	
	}	
	return T;
}


//////Modelo y parametros de crecimiento:
float Radius(float r1, float t1,float t2)
{
	return sqrt(pow(r1,2.0) + V*(t2-t1));
}

float ContactTime(float r11,float t11,float r12,float t12,float d2)
{
	
	float t12v,t11v;
	float T,k;
	
	t12v = t12 * V;
	t11v = t11 * V;
	k=d2 - pow(r11,2.0) + pow(r12,2.0) - t12v + t11v;
	T=(-r11 + t12v + pow(k,2.0)/(4.0*d2))/V;
	return T;
}

float ContacTime1(float r1,float t1,float d)
{
	float T;
	T=(pow(d,2.0)-pow(r1,2.0))/V + t1;
	return T;
}
