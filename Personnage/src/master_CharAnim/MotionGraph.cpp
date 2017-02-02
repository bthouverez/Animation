#include "MotionGraph.h"

void MotionGraph::init() {

    // Lecture des bvh du dossier
    DIR * rep = opendir(dir);
    if (rep != NULL) {
        struct dirent * ent;            
        while ((ent = readdir(rep)) != NULL) {
            if(ent->d_name[0] != '.') {
                BVH bvh;
                std::string s1(dir);
                std::string s2(ent->d_name);
                std::string s3 = s1 + s2;
                std::cout << s3 << std::endl;
                bvh.init(s3.c_str());
                m_BVH.push_back(bvh);
            }
        }
        closedir(rep);
    }

    // Création des noeuds du graphe
    for(unsigned int ii = 0; ii < m_BVH.size(); ii++) {
        BVH bvh = m_BVH[ii];

        for(int jj = 0; jj < bvh.getNumberOfFrame(); jj++) {
            // pour chaque pose de chaque bvh, on crée un noeud
            GrapheNode gn;
            gn.id_bvh = ii;
            gn.frame = jj;
            gn.ids_next = std::vector<GrapheNodeID>();
            // Ce noeud a un fils, la frame suivante, si ce n'est pas la derniere frame du BVH
            if(jj != bvh.getNumberOfFrame()-1) gn.ids_next.push_back(m_GrapheNode.size()+1);
            m_GrapheNode.push_back(gn);
        }
    }

    // Création des fils
    // Parcours des noeuds crées 
    for(unsigned int ii = 0; ii < m_GrapheNode.size(); ii++) {
        GrapheNode gn = m_GrapheNode[ii];

        // Squelette du noeud courant
        Skeleton ske;
        ske.init(m_BVH[gn.id_bvh]);
        ske.setPose(m_BVH[gn.id_bvh], gn.frame);

        for(unsigned int jj = 0; jj < m_GrapheNode.size(); jj++) {

            GrapheNode gn_cmp = m_GrapheNode[jj];
            // Pas besoin de faire la recherche sur le meme bvh
            if(gn_cmp.id_bvh == gn.id_bvh) continue;


            // Squelette du noeud à comparer
            Skeleton ske_cmp;
            ske_cmp.init(m_BVH[gn_cmp.id_bvh]);
            ske_cmp.setPose(m_BVH[gn_cmp.id_bvh], gn_cmp.frame);

            float dist = distance(ske, ske_cmp);
            if(dist <= 125.f) {
                // On ajoute un fils si la distance est inférieure à un certain seuil
                std::cout << "distance (" << gn.id_bvh << ", " << gn.frame << ") , (" << gn_cmp.id_bvh << ", " << gn_cmp.frame << ") = " << dist << std::endl;
                m_GrapheNode[ii].ids_next.push_back(jj);
                break; // On ne garde qu'un fils, un seul lien entre un noeud d'un bvh et un autre bvh
            }
        }
    }
}

void MotionGraph::print() {
    for(unsigned int ii = 0; ii < m_GrapheNode.size(); ii++) {
        GrapheNode gn = m_GrapheNode[ii];
        std::cout << "##########################################" << std::endl<< std::endl;
        std::cout << "Noeud [" << ii << "]" << std::endl;
        std::cout << "  -> bvh  : " << gn.id_bvh << std::endl;
        std::cout << "  -> pose : " << gn.frame << std::endl << std::endl;

        for(auto next: gn.ids_next) {

            std::cout << "   === Fils [" << next << "]" << std::endl;
            std::cout << "      -> bvh  : " << m_GrapheNode[next].id_bvh << std::endl;
            std::cout << "      -> pose : " << m_GrapheNode[next].frame << std::endl << std::endl;
        }
        std::cout << "" << std::endl;
        std::cout << "" << std::endl;
    }
}