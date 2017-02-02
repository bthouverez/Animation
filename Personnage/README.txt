## Animation de personnage

## Bastien Thouverez 10903295


Compilation:

	premake4 gmake
	make -j7


Execution:

	./bin/master_CharAnim <bvh_start_file> [<bvh_directory>]
	
	<bvh_start_file> : fichier BVH  pour l'animation de départ du personnage
	<bvh_directory> : chemin vers répertoire contenant des fichiers BVH pour Motion graph


Exemple d'execution:

	./bin/master_CharAnim data/bvh/motionTest/ data/bvh/motionTest/courir.bvh 
	


Travail effectué:

	- Interpolation simple et interpolation Quaternion

		Résultat visible en modifiant la valeur du #define COMP_INTERP dans le fichier CharAnimViewer.h en mettant à 1 -> affichera 6 squelettes comme dans le sujet
		
		Execution conseillée:

			./bin/master_CharAnim data/bvh/motionTest/courir.bvh 

	
	- Controlleur d'animation

		Le personnage peut être controlé avec ZQSD


	- Motion Graph

		La construction semble bonne mais très longue.
		Résultat appréciable avec l'exemple d'execution présenté ci-dessus, travaille sur un dossier ne contenant que 2 BVH ...
		Pas de navigation dans le graphe.