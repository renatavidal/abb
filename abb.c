
#include "abb.h"

#include <stdbool.h>
#include <stdlib.h>

#define EXITO 0
#define ERROR -1

abb_t* arbol_crear(abb_comparador comparador, abb_liberar_elemento destructor){
	abb_t* arbol= malloc(sizeof(abb_t));
	if(!arbol) return NULL;
	arbol->nodo_raiz= NULL;
	arbol->comparador= comparador;
	arbol->destructor = destructor;
	return arbol;
}


bool arbol_vacio(abb_t* arbol){
	if(!arbol) return true;
	if(arbol->nodo_raiz) return false;
	return true;
}

void* arbol_raiz(abb_t* arbol){
	if(!arbol) return NULL;
	if(arbol_vacio(arbol)) return NULL;
	if(!(arbol->nodo_raiz)) return NULL;
	return arbol->nodo_raiz->elemento;
}

int insertar_elemento(nodo_abb_t* nodo_insertado, void* elemento){
	nodo_abb_t* nodo= calloc(1, sizeof(nodo_abb_t));
	if(!nodo) return ERROR;
	nodo_insertado = nodo;
	nodo_insertado->elemento = elemento;
	return EXITO;
}

// recorre recursivamente los nodos e inserta en caso de encontrar una derecha libre con elemento mayor o izquierda libre con elemento menor.
int recorrer_nodos(abb_t* arbol, nodo_abb_t* nodo, void* elemento){
	if(!nodo) return insertar_elemento(nodo, elemento);
	else if(arbol->comparador(elemento, nodo->elemento)== 1)
		recorrer_nodos(arbol, nodo->izquierda, elemento);
	else if(arbol->comparador(elemento, nodo->elemento)== -1)
		recorrer_nodos(arbol, nodo->derecha, elemento);
	else if(arbol->comparador(elemento, nodo->elemento)== 0)
		recorrer_nodos(arbol, nodo->izquierda, elemento);
	return ERROR;
}

// si el elemento es igual utilizo la convencion de agregarlo a la derecha
int arbol_insertar(abb_t* arbol, void* elemento){
	if(!arbol) return ERROR;
	if(!(arbol->nodo_raiz)) return insertar_elemento (arbol->nodo_raiz, elemento);
	return recorrer_nodos(arbol, arbol->nodo_raiz, elemento);
}
// recorre recursivamente los nodos hasta encontrar el que tiene el elemento buscando, si no lo encuentra devuelve null
void* encontrar_nodo(abb_t* arbol, nodo_abb_t* nodo, void* elemento){
	if(!nodo) return NULL;
	else if(arbol->comparador(elemento, nodo->elemento)== 1)
		encontrar_nodo(arbol, nodo->izquierda, elemento);
	else if(arbol->comparador(elemento, nodo->elemento)== -1)
		encontrar_nodo(arbol, nodo->derecha, elemento);
	else if(arbol->comparador(elemento, nodo->elemento)== 0)
		return nodo;
	return NULL;
}

void* arbol_buscar(abb_t* arbol, void* elemento){
	if(arbol_vacio(arbol)) return NULL;
	nodo_abb_t* nodo_buscado=encontrar_nodo(arbol, arbol->nodo_raiz, elemento);
	if(!nodo_buscado) return NULL;
	else return nodo_buscado->elemento;
}
nodo_abb_t* predecesor_inorden(nodo_abb_t* nodo){
	if(!(nodo->derecha->derecha)) return nodo;
	return predecesor_inorden(nodo->derecha);
}
//chequea si el sucesor tiene un hijo a su derecha (ya que mas no puede tener, y tampoco a su izquierda)
// lo elimina si es necesario  y devuelve el nuevo nodo o null si no tenia hijos, 
void liberar_predecesor(nodo_abb_t* nodo_padre, nodo_abb_t* nodo_hijo){
	if(nodo_hijo->izquierda){
		nodo_padre->derecha= nodo_hijo->izquierda;
		free(nodo_hijo);
	}
	else{
		free(nodo_hijo);
		nodo_padre->derecha=NULL;
	}
}

nodo_abb_t* nodo_borrar(abb_t* arbol, nodo_abb_t* nodo, void* elemento){
	if(!nodo) return NULL;
	if(arbol->comparador(elemento, nodo->elemento)== 0){
		if(!(nodo->derecha) && (!nodo->izquierda)){
			free(nodo);
			return NULL;
		}
		else if(nodo->derecha && nodo->izquierda){
			nodo_abb_t* nodo_padre= predecesor_inorden(nodo->izquierda);
			nodo_abb_t* reemplazo=nodo_padre->derecha;
			liberar_predecesor(nodo_padre, reemplazo);
			free(nodo);
			return reemplazo;
		}
		else{
			nodo_abb_t* nodo_nuevo= nodo->derecha? nodo->derecha: nodo->izquierda;
			free(nodo);
			return nodo_nuevo;
		}
	}
	if(arbol->comparador(elemento, nodo->elemento)== 1){
		nodo->derecha = nodo_borrar(arbol, nodo->derecha, elemento);
	}
	else nodo->izquierda= nodo_borrar(arbol, nodo->izquierda, elemento);
}

int arbol_borrar(abb_t* arbol, void* elemento){
	if(arbol_vacio(arbol)) return ERROR;
	if(!arbol->comparador) return ERROR;
	arbol->nodo_raiz=nodo_borrar(arbol, arbol->nodo_raiz, elemento);
	return EXITO;
}

int elementos_array(void* elemento, void** array, size_t tamanio_array, int* posicion_actual){
	if(*posicion_actual < (int)tamanio_array)
	array[*posicion_actual]= elemento;
}

void in_orden(nodo_abb_t* nodo, void** array, size_t tamanio_array, int* posicion_actual){
	if(!nodo) return;
	if(nodo->izquierda) in_orden(nodo->izquierda, array, tamanio_array, posicion_actual);
	if(nodo){
		elementos_array(nodo->elemento, array, tamanio_array, posicion_actual);
		*posicion_actual++;
	}
	if(nodo->derecha) in_orden(nodo->derecha, array, tamanio_array, posicion_actual);
}

size_t arbol_recorrido_inorden(abb_t* arbol, void** array, size_t tamanio_array){
	size_t cantidad_leidos=0;
	if(arbol->nodo_raiz) in_orden(arbol->nodo_raiz, array, tamanio_array, &cantidad_leidos);
	return cantidad_leidos;
}

void preorden(nodo_abb_t* nodo, void** array, size_t tamanio_array, int* posicion_actual){
	if(!nodo) return;
	if(nodo){
		elementos_array(nodo->elemento, array, tamanio_array, posicion_actual);
		*posicion_actual++;
	}
	if(nodo->izquierda) preorden(nodo->izquierda, array, tamanio_array, posicion_actual);
	if(nodo->derecha) preorden(nodo->derecha, array, tamanio_array, posicion_actual);
}

size_t arbol_recorrido_preorden(abb_t* arbol, void** array, size_t tamanio_array){
	size_t cantidad_leidos=0;
	if(arbol->nodo_raiz) preorden(arbol->nodo_raiz, array, tamanio_array, &cantidad_leidos);
	return cantidad_leidos;
}

void postorden(nodo_abb_t* nodo, void** array, size_t tamanio_array, int* posicion_actual){
	if(!nodo) return;
	if(nodo->izquierda) postorden(nodo->izquierda, array, tamanio_array, posicion_actual);
	if(nodo->derecha) postorden(nodo->derecha, array, tamanio_array, posicion_actual);
	if(nodo){
		elementos_array(nodo->elemento, array, tamanio_array, posicion_actual);
		*posicion_actual++;
	}
}

size_t arbol_recorrido_postorden(abb_t* arbol, void** array, size_t tamanio_array){
	size_t cantidad_leidos=0;
	if(arbol->nodo_raiz) postorden(arbol->nodo_raiz, array, tamanio_array, &cantidad_leidos);
	return cantidad_leidos;
}


/*
 * Destruye el arbol liberando la memoria reservada por el mismo.
 * Adicionalmente invoca el destructor con cada elemento presente en
 * el arbol.
 */

void destruir_nodos(abb_t* arbol, nodo_abb_t* nodo){
	if(!nodo) return;
	destruir_nodos(arbol, nodo->derecha);
	destruir_nodos(arbol, nodo->izquierda);
	arbol->destructor(nodo->elemento);
	free(nodo);
}

void arbol_destruir(abb_t* arbol){
	if(!arbol) return;
	destruir_nodos(arbol, arbol->nodo_raiz);
}