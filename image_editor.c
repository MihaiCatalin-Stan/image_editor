// 315CAb_StanMihai-Catalin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
typedef struct {
	int magic, *sizes, **pixels, range, nrp, **selected, *ssize, x1, y1, x2, y2;
} pic;
// Structura ce retine poza dupa formatul ppm/pgm
// sizes - vector ce retine dimensiunile pozei
// ssize - vector ce retine dimensiunile selectiei
// pixels - matrice ce retine valorile pozei
// selected - matrice ce retine valorile selectiei
// magic - codul pozei / tipul pozei
// range - valoarea maxima din poza
// nrp - numarul de canale de culoare
// x1/y1/x2/y2 - coordonatele selectiei

int magicword(FILE *file)
{
	char buffer[100];
	while (fgets(buffer, sizeof(buffer), file)) {
		if (buffer[0] == '#') {
			continue;
		} else {
			int a = buffer[1] - '0';
			return a;
		}
	}
	return 0;
}

/* Functie ce sare peste comentarii, citeste o linie si returneaza valoarea
tipului pozei, transformand-o din char in int */

int *sizes(FILE *file)
{
	char buffer[100];
	int *wh = (int *)calloc(sizeof(int), 2);
	while (fgets(buffer, sizeof(buffer), file)) {
		if (buffer[0] == '#') {
			continue;
		} else {
			buffer[strlen(buffer) - 1] = '\0';
			int i = 0;
			while (buffer[i] != ' ') {
				wh[0] = wh[0] * 10 + (buffer[i] - '0');
				i++;
			}
			i++;
			while (buffer[i] != '\0') {
				wh[1] = wh[1] * 10 + (buffer[i] - '0');
				i++;
			}
			return wh;
		}
	}
	return 0;
}

/* Functie similara cu cea de magicword, doar ca va returna vectorul
ce stocheaza dimensiunile pozei */

int range(FILE *file)
{
	char buffer[100];
	int a = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		if (buffer[0] == '#') {
			continue;
		} else {
			buffer[strlen(buffer) - 1] = '\0';
			int i = 0;
			while (buffer[i] != '\0') {
				a = a * 10 + (buffer[i] - '0');
				i++;
			}
			return a;
		}
	}
	return 0;
}

/* Functie similara cu cele anterioare care va returna valoarea
maxima din matricea de pixeli */

int **pixel_mat(FILE *file, pic *loading)
{
	char buffer[100];
	int nrc = (*loading).sizes[0] * (*loading).nrp, **matrix = NULL, k = 0;
	matrix = (int **)calloc((*loading).sizes[1], sizeof(int *));
	if (!matrix)
		printf("EROARE DE ALOCARE\n");
	for (int i = 0 ; i < (*loading).sizes[1] ; i++) {
		matrix[i] = (int *)calloc(nrc, sizeof(int));
		if (!matrix[i])
			printf("EROARE DE ALOCARE\n");
	}
	fscanf(file, "%s", buffer);
	while (buffer[0] == '#') {
		fgets(buffer, sizeof(buffer), file);
		fscanf(file, "%s", buffer);
	}
	while (buffer[k] != '\0' && buffer[k] != ' ') {
		matrix[0][0] = matrix[0][0] * 10 + (buffer[k] - '0');
		k++;
	}
	for (int j = 1 ; j < nrc ; j++)
		if (!fscanf(file, "%d ", &matrix[0][j]))
			printf("eroare ");
	for (int i = 1 ; i < (*loading).sizes[1] ; i++)
		for (int j = 0 ; j < nrc ; j++)
			if (!fscanf(file, "%d ", &matrix[i][j]))
				printf("eroare ");
	return matrix;
}

/* Functie pentru citirea matricii de pixeli din fisiere de tip txt
verifica prezenta comentariilor, transforma primul element citit din
char in int, si citeste restul de elemente intr-o matrice alocata
dinamica pe care o returneaza */

int **pixel_mat_bin(FILE *file, pic *loading)
{
	int nrc = (*loading).sizes[0] * (*loading).nrp, **matrix;
	matrix = (int **)malloc(sizeof(int *) * (*loading).sizes[1]);
	if (!matrix)
		printf("EROARE DE ALOCARE\n");
	for (int i = 0 ; i < (*loading).sizes[1] ; i++) {
		matrix[i] = (int *)malloc(sizeof(int) * nrc);
		if (!matrix[i])
			printf("EROARE DE ALOCARE\n");
	}
	unsigned char buffy;
	for (int i = 0 ; i < (*loading).sizes[1] ; i++)
		for (int j = 0 ; j < (*loading).sizes[0] * (*loading).nrp ; j++) {
			fread(&buffy, sizeof(unsigned char), 1, file);
			matrix[i][j] = (int)buffy;
		}
	return matrix;
}

/* Functie similara cu cea anterioara, va fi folosita pentru citirea
elementelor din fisierele binare */

pic *load(char name[100])
{
	pic *loading = (pic *)malloc(sizeof(pic));
	FILE *file = fopen(name, "rtb");
	if (!file) {
		printf("Failed to load %s\n", name);
		free(loading);
		loading = NULL;
		return loading;
	}
	(*loading).ssize = (int *)malloc(2 * sizeof(int));
	(*loading).magic = magicword(file);
	(*loading).sizes = sizes(file);
	if ((*loading).magic == 2 || (*loading).magic == 5) {
		(*loading).range = range(file);
		(*loading).nrp = 1;
	} else if ((*loading).magic == 3 || (*loading).magic == 6) {
		(*loading).range = range(file);
		(*loading).nrp = 3;
	} else {
		(*loading).range = 1;
		(*loading).nrp = 1;
	}
	if ((*loading).magic < 4)
		(*loading).pixels = pixel_mat(file, loading);
	else
		(*loading).pixels = pixel_mat_bin(file, loading);
	(*loading).ssize[1] = (*loading).sizes[1];
	(*loading).ssize[0] = (*loading).sizes[0];
	(*loading).selected = (int **)malloc((*loading).ssize[1] * sizeof(int *));
	for (int i = 0 ; i < (*loading).ssize[1] ; i++) {
		(*loading).selected[i] = (int *)malloc((*loading).ssize[0] *
		(*loading).nrp * sizeof(int));
		if (!(*loading).selected[i])
			printf("eroare");
	}
	(*loading).x1 = 0;
	(*loading).y1 = 0;
	(*loading).x2 = (*loading).ssize[0];
	(*loading).y2 = (*loading).ssize[1];
	for (int i = 0 ; i < (*loading).ssize[1] ; i++)
		for (int j = 0; j < (*loading).ssize[0] * (*loading).nrp ; j++)
			(*loading).selected[i][j] =  (*loading).pixels[i][j];
	printf("Loaded %s\n", name);
	fclose(file);
	return loading;
}

// Functia care incarca in structura datele unei poze de tip .pgm sau .ppm

int transform_parameter(char par[10])
{
	int k = 0, x = 0;
	while (par[k] != '\0') {
		x = x * 10 + (par[k] - '0');
		k++;
	}
	return x;
}

// Functie ce transforma un sir de caractere intr-o valoare de tip intreg

int read_parameters(char par[10], int *x1, int *y1, int *x2, int *y2)
{
	if ((par[0] < '0' || par[0] > '9') && par[0] != '-')
		return 0;
	*x1 = transform_parameter(par);
	scanf("%c", &par[0]);
	if (par[0] == ' ') {
		scanf("%s", par);
		if ((par[0] < '0' || par[0] > '9') && par[0] != '-')
			return 0;
		*y1 = transform_parameter(par);
		scanf("%c", &par[0]);
		if (par[0] == ' ') {
			scanf("%s", par);
			if ((par[0] < '0' || par[0] > '9') && par[0] != '-')
				return 0;
			*x2 = transform_parameter(par);
			scanf("%c", &par[0]);
			if (par[0] == ' ') {
				scanf("%s", par);
				if ((par[0] < '0' || par[0] > '9') && par[0] != '-')
					return 0;
				*y2 = transform_parameter(par);
				scanf("%c", &par[0]);
				if (par[0] == '\n')
					return 1;
				else
					return 0;
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

// Functie pentru citirea coordonatelor operatiei de SELECT

void select_all(pic *loaded)
{
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}
	(*loaded).x1 = 0;
	(*loaded).y1 = 0;
	(*loaded).x2 = (*loaded).sizes[0];
	(*loaded).y2 = (*loaded).sizes[1];
	if ((*loaded).selected) {
		for (int i = 0 ; i < (*loaded).ssize[1] ; i++)
			free((*loaded).selected[i]);
		free((*loaded).selected);
		(*loaded).selected = NULL;
	}
	(*loaded).ssize[1] = (*loaded).sizes[1];
	(*loaded).ssize[0] = (*loaded).sizes[0];
	(*loaded).selected = (int **)malloc((*loaded).ssize[1] * sizeof(int *));
	for (int i = 0 ; i < (*loaded).ssize[1] ; i++) {
		(*loaded).selected[i] = (int *)malloc((*loaded).ssize[0] *
		(*loaded).nrp * sizeof(int));
		if (!(*loaded).selected[i])
			printf("eroare");
	}
	for (int i = 0 ; i < (*loaded).ssize[1] ; i++)
		for (int j = 0 ; j < (*loaded).ssize[0] * (*loaded).nrp ; j++)
			(*loaded).selected[i][j] =  (*loaded).pixels[i][j];
	printf("Selected ALL\n");
}

/* Functie pentru operatie de SELECT ALL, elibereaza selectia anterioara,
o realoca si memoreaza toti pixelii din matricea de pixeli in
matricea selectiei */

void select_pic(pic *loaded)
{
	char par[10];
	scanf("%s", par);
	if ((par[0] >= '0' && par[0] <= '9') || par[0] == '-') {
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0, k = 0;
		if (!read_parameters(par, &x1, &y1, &x2, &y2)) {
			if (!loaded) {
				printf("No image loaded\n");
				return;
			}
			printf("Invalid command\n");
			return;
		}
			if (x1 > x2) {
				k = x1;
				x1 = x2;
				x2 = k;
			}
			if (y1 > y2) {
				k = y1;
				y1 = y2;
				y2 = k;
			}
		if (!loaded) {
			printf("No image loaded\n");
			return;
		}
		if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) {
			printf("Invalid set of coordinates\n");
			return;
		}
		if (x1 == x2 || y1 == y2) {
			printf("Invalid set of coordinates\n");
			return;
		}
		if (y2 > (*loaded).sizes[1] || x2 > (*loaded).sizes[0]) {
			printf("Invalid set of coordinates\n");
			return;
		}
		(*loaded).x1 = x1;
		(*loaded).y1 = y1;
		(*loaded).x2 = x2;
		(*loaded).y2 = y2;
		if ((*loaded).selected) {
			for (int i = 0 ; i < (*loaded).ssize[1] ; i++)
				free((*loaded).selected[i]);
			free((*loaded).selected);
			(*loaded).selected = NULL;
		}
		(*loaded).ssize[1] = y2 - y1;
		(*loaded).ssize[0] = x2 - x1;
		(*loaded).selected = (int **)calloc((*loaded).ssize[1], sizeof(int *));
		for (int i = 0 ; i < (*loaded).ssize[1] ; i++) {
			(*loaded).selected[i] = (int *)calloc((*loaded).ssize[0] *
			(*loaded).nrp, sizeof(int));
			if (!(*loaded).selected[i])
				printf("eroare");
		}
		for (int i = y1 ; i < y2 ; i++)
			for (int j = x1 * (*loaded).nrp ; j < x2 * (*loaded).nrp ; j++)
				(*loaded).selected[i - y1][j - (x1 * (*loaded).nrp)] =
				(*loaded).pixels[i][j];
		printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
	} else {
		select_all(loaded);
	}
}

/* Functie pentru operatia de SELECT, interschimba coordonatele selectiei
in cazul in care nu sunt in ordine crescatoare, elibereaza selectia anterioara
si aloca o noua matrice de dimensiunile cerute si cu valorile selectate */

int pow2(int y)
{
	while (!(y % 2))
		y /= 2;
	if (y == 1)
		return 1;
	else
		return 0;
}

// Functie care verifica daca un numar 'y' este o putere a lui 2

void histogram(pic *loaded)
{
	int y, x, frm = 0, starnum, *histogram;
	char par[20];
	scanf("%c", &par[0]);
	if (par[0] == '\n') {
		if (!loaded) {
			printf("No image loaded\n");
			return;
		}
		printf("Invalid command\n");
		return;
	}
	scanf("%d", &x);
	scanf("%c", &par[0]);
	if (par[0] == '\n') {
		printf("Invalid command\n");
		return;
	}
	scanf("%d", &y);
	scanf("%c", &par[0]);
	if (par[0] == ' ') {
		fgets(par, sizeof(par), stdin);
		printf("Invalid command\n");
		return;
	}
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}
	if ((*loaded).magic == 3 || (*loaded).magic == 6) {
		printf("Black and white image needed\n");
		return;
	}
	if (!pow2(y)) {
		printf("Invalid set of parameters\n");
		return;
	}
	histogram = (int *)calloc(y, sizeof(int));
	for (int i = 0 ; i < (*loaded).ssize[1] ; i++)
		for (int j = 0 ; j < (*loaded).ssize[0] ; j++)
			histogram[(*loaded).selected[i][j] / (256 / y)]++;
	for (int i = 0 ; i < y ; i++)
		if (histogram[i] > frm)
			frm = histogram[i];
	for (int i = 0 ; i < y ; i++) {
		starnum = (histogram[i] * x) / frm;
		printf("%d\t|\t", starnum);
		for (int j = 0 ; j < starnum ; j++)
			printf("*");
		printf("\n");
	}
	free(histogram);
}

// Functie de afisare a histogramei unei poze grayscale

void equalize(pic *loaded)
{
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}
	if ((*loaded).magic == 3 || (*loaded).magic == 6) {
		printf("Black and white image needed\n");
		return;
	}
	int nrc = (*loaded).sizes[0] * (*loaded).nrp, freq[256] = {};
	double **equalized, total[256];
	equalized = (double **)malloc((*loaded).sizes[1] * sizeof(double *));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		equalized[i] = (double *)malloc(nrc * sizeof(double));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		for (int j = 0 ; j < nrc ; j++)
			freq[(*loaded).pixels[i][j]]++;
	total[0] = freq[0];
	for (int i = 1 ; i < 256 ; i++)
		total[i] = total[i - 1] + freq[i];
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		for (int j = 0 ; j < nrc ; j++) {
			equalized[i][j] = total[(*loaded).pixels[i][j]] * 255 /
			(nrc * (*loaded).sizes[1]);
			equalized[i][j] = round(equalized[i][j]);
			if (equalized[i][j] < 0)
				equalized[i][j] = 0;
			else if (equalized[i][j] > 255)
				equalized[i][j] = 255;
		}
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		for (int j = 0 ; j < nrc ; j++)
			(*loaded).pixels[i][j] = (int)equalized[i][j];
	for (int i = (*loaded).y1 ; i < (*loaded).y2 ; i++)
		for (int j = (*loaded).x1 * (*loaded).nrp ; j < (*loaded).x2 *
		(*loaded).nrp ; j++)
			(*loaded).selected[i - (*loaded).y1][j - ((*loaded).x1 *
			(*loaded).nrp)] = (*loaded).pixels[i][j];
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		free(equalized[i]);
	free(equalized);
	printf("Equalize done\n");
}

// Functia de egalizare a valorilor matricii de pixeli pentru o poza grayscale

void crop(pic *loaded)
{
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		free((*loaded).pixels[i]);
	free((*loaded).pixels);
	(*loaded).sizes[1] = (*loaded).ssize[1];
	(*loaded).sizes[0] = (*loaded).ssize[0];
	(*loaded).x1 = 0;
	(*loaded).y1 = 0;
	(*loaded).x2 = (*loaded).ssize[0];
	(*loaded).y2 = (*loaded).ssize[1];
	(*loaded).pixels = (int **)malloc(sizeof(int *) * (*loaded).ssize[1]);
	if (!(*loaded).pixels)
		printf("EROARE DE ALOCARE\n");
	for (int i = 0 ; i < (*loaded).ssize[1] ; i++) {
		(*loaded).pixels[i] = (int *)malloc(sizeof(int) *
		(*loaded).ssize[0] * (*loaded).nrp);
		if (!(*loaded).pixels[i])
			printf("EROARE DE ALOCARE\n");
	}
	for (int i = 0 ; i < (*loaded).ssize[1] ; i++)
		for (int j = 0 ; j < (*loaded).ssize[0] * (*loaded).nrp ; j++)
			(*loaded).pixels[i][j] = (*loaded).selected[i][j];
	printf("Image cropped\n");
}

/* Functie ce elibereaza matricea de pixeli curenta,
o aloca si o inlocuieste cu matricea pixelilor selectati */

void save(pic *loaded)
{
	char buffer[100] = {}, *p, name[100];
	int count = 0, txt = 0;
	fgets(buffer, sizeof(buffer), stdin);
	p = strtok(buffer, " ");
	while (p) {
		if (p && count == 0) {
			strcpy(name, p);
			if (name[strlen(name) - 1] == '\n')
				name[strlen(name) - 1] = '\0';
		} else if (p && count == 1) {
			txt = 1;
		}
		count++;
		p = strtok(NULL, " ");
	}
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}
	FILE *saved;
	if (txt)
		saved = fopen(name, "wt");
	else
		saved = fopen(name, "wb");
	if (txt && (*loaded).magic > 3)
		fprintf(saved, "P%d\n", (*loaded).magic - 3);
	else if (!txt && (*loaded).magic < 4)
		fprintf(saved, "P%d\n", (*loaded).magic + 3);
	else
		fprintf(saved, "P%d\n", (*loaded).magic);
	fprintf(saved, "%d %d\n", (*loaded).sizes[0], (*loaded).sizes[1]);
	if ((*loaded).magic != 1 && (*loaded).magic != 4)
		fprintf(saved, "%d\n", (*loaded).range);
	int i, j;
	if (txt)
		for (i = 0 ; i < (*loaded).sizes[1] ; i++) {
			for (j = 0 ; j < (*loaded).sizes[0] * (*loaded).nrp ; j++)
				fprintf(saved, "%d ", (*loaded).pixels[i][j]);
			fprintf(saved, "\n");
		}
	else
		for (i = 0 ; i < (*loaded).sizes[1] ; i++) {
			for (j = 0 ; j < (*loaded).sizes[0] * (*loaded).nrp ; j++) {
				char buffy;
				buffy = (unsigned char)(*loaded).pixels[i][j];
				fwrite(&buffy, sizeof(unsigned char), 1, saved);
			}
		}
	printf("Saved %s\n", name);
	fclose(saved);
}

/* Functie de salvare a unei poze de tip .ppm sau .pgm intr-un fisier
atat text cat si binar in functie de datele introduse */

int **edge_detection(pic *loaded) //mem
{
	int **edged, **a = (*loaded).pixels;
	int newr, newg, newb, imin, jmin, imax, jmax;
	edged = (int **)malloc((*loaded).sizes[1] * sizeof(int *));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		edged[i] = (int *)malloc((*loaded).sizes[0] *
		(*loaded).nrp * sizeof(int));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		for (int j = 0 ; j < (*loaded).sizes[0] * (*loaded).nrp ; j++)
			edged[i][j] = a[i][j];
	if ((*loaded).y1 == 0)
		imin = (*loaded).y1 + 1;
	else
		imin = (*loaded).y1;
	if ((*loaded).y2 == (*loaded).sizes[1])
		imax = (*loaded).y2 - 1;
	else
		imax = (*loaded).y2;
	if ((*loaded).x1 == 0)
		jmin = (*loaded).x1 + 1;
	else
		jmin = (*loaded).x1;
	if ((*loaded).x2 == (*loaded).sizes[0])
		jmax = (*loaded).x2 - 1;
	else
		jmax = (*loaded).x2;
	for (int i = imin ; i < imax ; i++)
		for (int j = jmin * (*loaded).nrp ; j < (jmax - 1) *
		(*loaded).nrp + 1 ; j++) {
			newr = (-1) * (a[i][j + 3] + a[i][j - 3] + a[i + 1][j]
			+ a[i - 1][j] + a[i + 1][j + 3] + a[i - 1][j - 3] + a[i + 1][j - 3]
			+ a[i - 1][j + 3]) + 8 * a[i][j];
			newg = (-1) * (a[i][j + 4] + a[i][j - 2] + a[i + 1][j + 1]
			+ a[i - 1][j + 1] + a[i + 1][j + 4] + a[i - 1][j - 2] +
			a[i + 1][j - 2] + a[i - 1][j + 4]) + 8 * a[i][j + 1];
			newb = (-1) * (a[i][j + 5] + a[i][j - 1] + a[i + 1][j + 2]
			+ a[i - 1][j + 2] + a[i + 1][j + 5] + a[i - 1][j - 1] +
			a[i + 1][j - 1] + a[i - 1][j + 5]) + 8 * a[i][j + 2];
			if (newr < 0)
				newr = 0;
			if (newg < 0)
				newg = 0;
			if (newb < 0)
				newb = 0;
			if (newr > 255)
				newr = 255;
			if (newg > 255)
				newg = 255;
			if (newb > 255)
				newb = 255;
			edged[i][j] = newr;
			edged[i][j + 1] = newg;
			edged[i][j + 2] = newb;
			}
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		free(a[i]);
	free(a);
	return edged;
}

/* Functie de aplicare a nucleului de imagine 'edge detection' pe o
zona selectata. Se transpun valorile pozei intr-o noua matrice
si se aplica nucleul pe zona selectata. In cazul selectiei intregii
imagini, se vor ignora marginile */

int **sharpen(pic *loaded) // mem
{
	int **sharpened, **a = (*loaded).pixels;
	int newr, newg, newb, imin, imax, jmin, jmax;
	sharpened = (int **)calloc((*loaded).sizes[1], sizeof(int *));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		sharpened[i] = (int *)calloc((*loaded).sizes[0] *
		(*loaded).nrp, sizeof(int));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		for (int j = 0 ; j < (*loaded).sizes[0] * (*loaded).nrp ; j++)
			sharpened[i][j] = a[i][j];
	if ((*loaded).y1 == 0)
		imin = (*loaded).y1 + 1;
	else
		imin = (*loaded).y1;
	if ((*loaded).y2 == (*loaded).sizes[1])
		imax = (*loaded).y2 - 1;
	else
		imax = (*loaded).y2;
	if ((*loaded).x1 == 0)
		jmin = (*loaded).x1 + 1;
	else
		jmin = (*loaded).x1;
	if ((*loaded).x2 == (*loaded).sizes[0])
		jmax = (*loaded).x2 - 1;
	else
		jmax = (*loaded).x2;
	for (int i = imin ; i < imax ; i++)
		for (int j = jmin * (*loaded).nrp ; j < (jmax - 1) *
		(*loaded).nrp + 1 ; j++) {
			newr = (-1) * (a[i][j + 3] + a[i][j - 3] + a[i + 1][j] +
			a[i - 1][j]) + 5 * a[i][j];
			newg = (-1) * (a[i][j + 4] + a[i][j - 2] + a[i + 1][j + 1]
			+ a[i - 1][j + 1]) + 5 * a[i][j + 1];
			newb = (-1) * (a[i][j + 5] + a[i][j - 1] + a[i + 1][j + 2]
			+ a[i - 1][j + 2]) + 5 * a[i][j + 2];
			if (newr < 0)
				newr = 0;
			if (newg < 0)
				newg = 0;
			if (newb < 0)
				newb = 0;
			if (newr > 255)
				newr = 255;
			if (newg > 255)
				newg = 255;
			if (newb > 255)
				newb = 255;
			sharpened[i][j] = newr;
			sharpened[i][j + 1] = newg;
			sharpened[i][j + 2] = newb;
			}
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		free(a[i]);
	free(a);
	return sharpened;
}

/* Functie de aplicare a nucleului de imagine 'sharpen'. Functioneaza
dupa acelasi principiu ca functia de 'edge detection' */

int **box_blur(pic *loaded) //mem
{
	double newr, newg, newb;
	int **blurred, **a = (*loaded).pixels, imin, jmin, imax, jmax;
	blurred = (int **)malloc((*loaded).sizes[1] * sizeof(int *));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		blurred[i] = (int *)malloc((*loaded).sizes[0] *
		(*loaded).nrp * sizeof(int));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		for (int j = 0 ; j < (*loaded).sizes[0] * (*loaded).nrp ; j++)
			blurred[i][j] = a[i][j];
	if ((*loaded).y1 == 0)
		imin = (*loaded).y1 + 1;
	else
		imin = (*loaded).y1;
	if ((*loaded).y2 == (*loaded).sizes[1])
		imax = (*loaded).y2 - 1;
	else
		imax = (*loaded).y2;
	if ((*loaded).x1 == 0)
		jmin = (*loaded).x1 + 1;
	else
		jmin = (*loaded).x1;
	if ((*loaded).x2 == (*loaded).sizes[0])
		jmax = (*loaded).x2 - 1;
	else
		jmax = (*loaded).x2;
	for (int i = imin ; i < imax ; i++)
		for (int j = jmin * (*loaded).nrp ; j < (jmax - 1) *
		(*loaded).nrp + 1 ; j++) {
			newr = round((a[i][j] + a[i][j - 3] + a[i][j + 3]
			+ a[i - 1][j] + a[i - 1][j - 3] + a[i - 1][j + 3]
			+ a[i + 1][j] + a[i + 1][j - 3] + a[i + 1][j + 3]) / 9);
			newg = round((a[i][j + 1] + a[i][j - 2] + a[i][j + 4]
			+ a[i - 1][j + 1] + a[i - 1][j - 2] + a[i - 1][j + 4]
			+ a[i + 1][j + 1] + a[i + 1][j - 2] + a[i + 1][j + 4]) / 9);
			newb = round((a[i][j + 2] + a[i][j - 1] + a[i][j + 5]
			+ a[i - 1][j + 2] + a[i - 1][j - 1] + a[i - 1][j + 5]
			+ a[i + 1][j + 2] + a[i + 1][j - 1] + a[i + 1][j + 5]) / 9);
			if (newr < 0)
				newr = 0;
			if (newg < 0)
				newg = 0;
			if (newb < 0)
				newb = 0;
			if (newr > 255)
				newr = 255;
			if (newg > 255)
				newg = 255;
			if (newb > 255)
				newb = 255;
			blurred[i][j] = (int)newr;
			blurred[i][j + 1] = (int)newg;
			blurred[i][j + 2] = (int)newb;
		}
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		free(a[i]);
	free(a);
	return blurred;
}

/* Functia de aplicare a nucleului de imagine 'box_blur'. Are acelasi mod
de functionalitate ca celelalte doua functii de apply */

int **gauss_blur(pic *loaded) //mem
{
	double newr, newg, newb;
	int **blurred, **a = (*loaded).pixels, imin, jmin, imax, jmax;
	blurred = (int **)malloc((*loaded).sizes[1] * sizeof(int *));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		blurred[i] = (int *)malloc((*loaded).sizes[0] *
		(*loaded).nrp * sizeof(int));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		for (int j = 0 ; j < (*loaded).sizes[0] * (*loaded).nrp ; j++)
			blurred[i][j] = a[i][j];
	if ((*loaded).y1 == 0)
		imin = (*loaded).y1 + 1;
	else
		imin = (*loaded).y1;
	if ((*loaded).y2 == (*loaded).sizes[1])
		imax = (*loaded).y2 - 1;
	else
		imax = (*loaded).y2;
	if ((*loaded).x1 == 0)
		jmin = (*loaded).x1 + 1;
	else
		jmin = (*loaded).x1;
	if ((*loaded).x2 == (*loaded).sizes[0])
		jmax = (*loaded).x2 - 1;
	else
		jmax = (*loaded).x2;
	for (int i = imin ; i < imax ; i++)
		for (int j = jmin * (*loaded).nrp ; j < (jmax - 1) *
		(*loaded).nrp + 1 ; j++) {
			newr = round((a[i][j] * 4 + (a[i][j - 3] + a[i][j + 3]
			+ a[i - 1][j] + a[i + 1][j]) * 2 + a[i - 1][j - 3] +
			a[i - 1][j + 3] + a[i + 1][j - 3] + a[i + 1][j + 3]) / 16);
			newg = round((a[i][j + 1] * 4 + (a[i][j - 2] + a[i][j + 4]
			+ a[i - 1][j + 1] + a[i + 1][j + 1]) * 2 + a[i - 1][j - 2]
			+ a[i - 1][j + 4] + a[i + 1][j - 2] + a[i + 1][j + 4]) / 16);
			newb = round((a[i][j + 2] * 4 + (a[i][j - 1] + a[i][j + 5]
			+ a[i - 1][j + 2] + a[i + 1][j + 2]) * 2 + a[i - 1][j - 1]
			+ a[i - 1][j + 5] + a[i + 1][j - 1] + a[i + 1][j + 5]) / 16);
			if (newr < 0)
				newr = 0;
			if (newg < 0)
				newg = 0;
			if (newb < 0)
				newb = 0;
			if (newr > 255)
				newr = 255;
			if (newg > 255)
				newg = 255;
			if (newb > 255)
				newb = 255;
			blurred[i][j] = (int)newr;
			blurred[i][j + 1] = (int)newg;
			blurred[i][j + 2] = (int)newb;
		}
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		free(a[i]);
	free(a);
	return blurred;
}

/* Ultima functie ce aplica nuclee de imagine. Folosita pentru 'gaussian_blur',
functioneaza identic cu cele mentionate anterior */

void apply(pic *loaded)
{
	char par[100];
	scanf("%c", &par[0]);
	if (par[0] == '\n') {
		if (!loaded) {
			printf("No image loaded\n");
			return;
		}
		printf("Invalid command\n");
		return;
	}
	fgets(par, sizeof(par), stdin);
	par[strlen(par) - 1] = '\0';
	//printf("%s", par);
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}
	if ((*loaded).magic == 2 || (*loaded).magic == 5) {
		printf("Easy, Charlie Chaplin\n");
		return;
	} else if (strcmp(par, "EDGE") == 0) {
		(*loaded).pixels = edge_detection(loaded);
		printf("APPLY %s done\n", par);
	} else if (strcmp(par, "SHARPEN") == 0) {
		(*loaded).pixels = sharpen(loaded);
		printf("APPLY %s done\n", par);
	} else if (strcmp(par, "BLUR") == 0) {
		(*loaded).pixels = box_blur(loaded);
		printf("APPLY %s done\n", par);
	} else if (strcmp(par, "GAUSSIAN_BLUR") == 0) {
		(*loaded).pixels = gauss_blur(loaded);
		printf("APPLY %s done\n", par);
	} else {
		printf("APPLY parameter invalid\n");
		return;
	}
}

// Functie pentru decizia nucleului aplicat la apelarea functiei APPLY

void free_pic(pic *loaded)
{
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		free((*loaded).pixels[i]);
	free((*loaded).pixels);
	free((*loaded).sizes);
	for (int i = 0 ; i < (*loaded).ssize[1] ; i++)
		free((*loaded).selected[i]);
	free((*loaded).selected);
	free((*loaded).ssize);
	free(loaded);
	loaded = NULL;
}

/* Functie pentru eliberarea din memorie a valorilor alocate dinamic in poza,
si in final a structurii in care este memorata */

int **rot90(pic *loaded)
{
	int **rotated;
	rotated = (int **)malloc((*loaded).sizes[1] * sizeof(int *));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		rotated[i] = (int *)malloc((*loaded).sizes[0] *
		(*loaded).nrp * sizeof(int));
	for (int i = 0 ; i < (*loaded).ssize[1] ; i++)
		for (int j = 0 ; j < (*loaded).ssize[0] * (*loaded).nrp ; j++)
			rotated[i][j] = (*loaded).pixels[i][j];
	if ((*loaded).x2 - (*loaded).x1 == (*loaded).y2 - (*loaded).y1) {
		if ((*loaded).magic == 3 || (*loaded).magic == 6) {
			for (int i = (*loaded).y1 ; i
			< (*loaded).y2 ; i++)
				for (int j = (*loaded).x1 ; j
				< (*loaded).x2 ; j--) {
					rotated[(*loaded).ssize[0] - j - 1] =
					(*loaded).selected[3 * i];
					rotated[(*loaded).ssize[0] - j - 2] =
					(*loaded).selected[3 * i + 1];
					rotated[(*loaded).ssize[0] - j - 3] =
					(*loaded).selected[3 * i + 2];
				}
		} else {
			for (int i = (*loaded).x1, k = 0 ; i
			< (*loaded).x2 - 1 ; i++, k++)
				for (int j = (*loaded).y2 - 1, p = 0 ; j
				>= (*loaded).y1 ; j--, p++)
					rotated[k][p] = (*loaded).selected[j][i];
		}
	} else {
		int aux = (*loaded).sizes[1];
		(*loaded).sizes[1] = (*loaded).sizes[0];
		(*loaded).sizes[0] = aux;
		(*loaded).ssize[1] = (*loaded).sizes[1];
		(*loaded).ssize[0] = (*loaded).sizes[0];
		aux = (*loaded).y2;
		(*loaded).y2 = (*loaded).x2;
		(*loaded).x2 = aux;
		if ((*loaded).magic == 3 || (*loaded).magic == 6) {
			for (int i = 0, k = 0 ; i
			< (*loaded).x2 - 1 ; i++, k++)
				for (int j = (*loaded).y2 - 1, p = 0 ; j
				>= (*loaded).y1 ; j--, p++) {
					rotated[(*loaded).ssize[0] - j - 1] =
					(*loaded).selected[3 * i];
					rotated[(*loaded).ssize[0] - j - 2] =
					(*loaded).selected[3 * i + 1];
					rotated[(*loaded).ssize[0] - j - 3] =
					(*loaded).selected[3 * i + 2];
				}
		} else {
			for (int i = (*loaded).x1, k = 0 ; i
			< (*loaded).x2 - 1 ; i++, k++)
				for (int j = (*loaded).y2 - 1, p = 0 ; j
				>= (*loaded).y1 ; j--, p++)
					rotated[k][p] = (*loaded).selected[j][i];
		}
	}
	return rotated;
}

/* Functie pentru rotirea la 90 (sau la -270) de grade a unei imagini
dupa tipul sau selectia acesteia. Se aloca o matrice de rotatie in care
sunt introduse valorile pozei curente, si apoi se fac modificari pe selectie.
In final se returneaza matricea cu valorile rotite */

int **rot180(pic *loaded)
{
	int **rotated;
	rotated = (int **)malloc((*loaded).sizes[1] * sizeof(int *));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		rotated[i] = (int *)malloc((*loaded).sizes[0] *
		(*loaded).nrp * sizeof(int));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		for (int j = 0 ; j < (*loaded).sizes[0] * (*loaded).nrp ; j++)
			rotated[i][j] = (*loaded).pixels[i][j];
	if ((*loaded).x2 - (*loaded).x1 == (*loaded).y2 - (*loaded).y1) {
		if ((*loaded).magic == 3 || (*loaded).magic == 6)
			for (int i = (*loaded).y2 - 1, k = 0 ; i
			>= (*loaded).y1 ; i--, k++)
				for (int j = (*loaded).x2, p = 0 ; j
				>= (*loaded).x1 ; j--, p++) {
					rotated[i][j * (*loaded).nrp - 3] =
					(*loaded).selected[k][p];
					rotated[i][j * (*loaded).nrp - 2] =
					(*loaded).selected[k][p + 1];
					rotated[i][j * (*loaded).nrp - 1] =
					(*loaded).selected[k][p + 2];
				}
		else
			for (int i = (*loaded).y2 - 1, k = 0 ; i
			>= (*loaded).y1 ; i--, k++)
				for (int j = (*loaded).x2 * (*loaded).nrp - 1, p = 0 ; j
				>= (*loaded).x1 * (*loaded).nrp ; j--, p++)
					rotated[i][j] = (*loaded).selected[k][p];
	} else {
		if ((*loaded).magic == 3 || (*loaded).magic == 6) {
			for (int i = (*loaded).y2 - 1, k = 0 ; i
			>= (*loaded).y1 ; i--, k++)
				for (int j = (*loaded).x2 * (*loaded).nrp - 1, p = 0 ; j
				>= (*loaded).x1 * (*loaded).nrp ; j -= 3, p++) {
					rotated[i][j - 2] = (*loaded).selected[k][p];
					rotated[i][j - 1] = (*loaded).selected[k][p + 1];
					rotated[i][j] = (*loaded).selected[k][p + 2];
				}
		} else {
			for (int i = (*loaded).y2 - 1, k = 0 ; i
			>= (*loaded).y1 ; i--, k++)
				for (int j = (*loaded).x2 * (*loaded).nrp - 1, p = 0 ; j
				>= (*loaded).x1 * (*loaded).nrp ; j--, p++)
					rotated[i][j] = (*loaded).selected[k][p];
		}
	}
	return rotated;
}

/* Functie pentru rotirea la 180 de grade a unei imagini
dupa tipul sau selectia acesteia */

int **rot270(pic *loaded)
{
	int **rotated;
	rotated = (int **)malloc((*loaded).sizes[1] * sizeof(int *));
	for (int i = 0 ; i < (*loaded).sizes[1] ; i++)
		rotated[i] = (int *)malloc((*loaded).sizes[0] *
		(*loaded).nrp * sizeof(int));
	for (int i = 0 ; i < (*loaded).ssize[1] ; i++)
		for (int j = 0 ; j < (*loaded).ssize[0] * (*loaded).nrp ; j++)
			rotated[i][j] = (*loaded).pixels[i][j];
	if ((*loaded).x2 - (*loaded).x1 == (*loaded).y2 - (*loaded).y1) {
		if ((*loaded).magic == 3 || (*loaded).magic == 6) {
			for (int i = (*loaded).y2 - 1, k = 0 ; i
			>= (*loaded).y1 ; i--, k++)
				for (int j = (*loaded).x2 * (*loaded).nrp - 1, p = 0 ; j
				>= (*loaded).x1 * (*loaded).nrp ; j--, p++) {
					rotated[(*loaded).x2 * (*loaded).nrp - 1 - i][j] =
					(*loaded).selected[k][p];
					rotated[(*loaded).x2 * (*loaded).nrp - i][j] =
					(*loaded).selected[k][p + 1];
					rotated[(*loaded).x2 * (*loaded).nrp + 1 - i][j] =
					(*loaded).selected[k][p + 2];
				}
			}
		else
			for (int i = (*loaded).x1, k = 0 ; i
			< (*loaded).x2 - 1 ; i++, k++)
				for (int j = (*loaded).y2 - 1, p = 0 ; j
				>= (*loaded).y1 ; j--, p++)
					rotated[j][i] = (*loaded).selected[k][p];
	} else {
		int aux = (*loaded).sizes[1];
		(*loaded).sizes[1] = (*loaded).sizes[0];
		(*loaded).sizes[0] = aux;
		(*loaded).ssize[1] = (*loaded).sizes[1];
		(*loaded).ssize[0] = (*loaded).sizes[0];
		aux = (*loaded).y2;
		(*loaded).y2 = (*loaded).x2;
		(*loaded).x2 = aux;
		if ((*loaded).magic == 3 || (*loaded).magic == 6) {
			for (int i = 0, k = 0 ; i
			< (*loaded).x2 - 1 ; i++, k++)
				for (int j = (*loaded).y2 - 1, p = 0 ; j
				>= (*loaded).y1 ; j--, p++) {
					rotated[(*loaded).x2 * (*loaded).nrp - 1 - i][j] =
					(*loaded).selected[k][p];
					rotated[(*loaded).x2 * (*loaded).nrp - i][j] =
					(*loaded).selected[k][p + 1];
					rotated[(*loaded).x2 * (*loaded).nrp + 1 - i][j] =
					(*loaded).selected[k][p + 2];
				}
		} else {
			for (int i = 0, k = 0 ; i
			< (*loaded).sizes[1] - 1 ; i++, k++)
				for (int j = (*loaded).sizes[0] - 1, p = 0 ; j
				>= 0 ; j--, p++)
					rotated[j][i] = (*loaded).selected[k][p];
		}
	}
	return rotated;
}

/* Functie pentru rotirea la 270 (sau la -90) de grade a unei imagini
dupa tipul sau selectia acesteia */

void rotate(pic *loaded)
{
	int angle;
	scanf("%d", &angle);
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}
	if (angle % 90) {
		printf("Unsupported rotation angle\n");
		return;
	}
	if (angle == 90 || angle == -270) {
		(*loaded).pixels = rot90(loaded);
		for (int i = (*loaded).y1 ; i < (*loaded).y2 ; i++)
			for (int j = (*loaded).x1 * (*loaded).nrp ; j
			< (*loaded).x2 * (*loaded).nrp ; j++) {
				(*loaded).selected[i - (*loaded).y1][j - ((*loaded).x1 *
				(*loaded).nrp)] = (*loaded).pixels[i][j];
			}
	}
	if (angle == 180 || angle == -180) {
		(*loaded).pixels = rot180(loaded);
		for (int i = (*loaded).y1 ; i < (*loaded).y2 ; i++)
			for (int j = (*loaded).x1 * (*loaded).nrp ; j
			< (*loaded).x2 * (*loaded).nrp ; j++) {
				(*loaded).selected[i - (*loaded).y1][j - ((*loaded).x1 *
				(*loaded).nrp)] = (*loaded).pixels[i][j];
			}
		for (int i = 0 ; i < (*loaded).ssize[1] ; i++) {
			for (int j = 0 ; j
			< (*loaded).ssize[0] * (*loaded).nrp ; j++)
				printf("%d ", (*loaded).selected[i][j]);
			printf("\n");
		}
	}
	if (angle == 270 || angle == -90) {
		(*loaded).pixels = rot270(loaded);
		for (int i = (*loaded).y1 ; i < (*loaded).y2 ; i++)
			for (int j = (*loaded).x1 * (*loaded).nrp ; j
			< (*loaded).x2 * (*loaded).nrp ; j++) {
				(*loaded).selected[i - (*loaded).y1][j - ((*loaded).x1 *
				(*loaded).nrp)] = (*loaded).pixels[i][j];
			}
	}
	printf("Rotated %d\n", angle);
}

/* Functie pentru selectarea unghiului de rotatie dorit al pozei si apelarea
functiei corespunzatoare acesteia */

int main(void)
{
	char op[50], name[100];
	pic *photo = NULL;
	do {
		scanf("%s", op);
		if (strcmp(op, "LOAD") == 0) {
			scanf("%s", name);
			if (photo)
				free_pic(photo);
			photo = load(name);
		} else if (strcmp(op, "SELECT") == 0) {
			select_pic(photo);
		} else if (strcmp(op, "QUIT") == 0) {
			exit(0);
		} else if (strcmp(op, "HISTOGRAM") == 0) {
			histogram(photo);
		} else if (strcmp(op, "EQUALIZE") == 0) {
			equalize(photo);
		} else if (strcmp(op, "CROP") == 0) {
			crop(photo);
		} else if (strcmp(op, "SAVE") == 0) {
			save(photo);
		} else if (strcmp(op, "APPLY") == 0) {
			apply(photo);
		} else if (strcmp(op, "EXIT") == 0) {
			if (!photo)
				printf("No image loaded\n");
			else
				free_pic(photo);
			exit(0);
		} else if (strcmp(op, "ROTATE") == 0) {
			rotate(photo);
		} else {
			printf("Invalid command\n");
			scanf("%c", &op[0]);
			if (op[0] != '\n')
				fgets(op, sizeof(op), stdin);
		}
		if (strcmp(op, "EXIT") == 0)
			exit(0);
	} while (1);
}
