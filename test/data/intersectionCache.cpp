// Boost Includes
#include <boost/numeric/ublas/matrix_sparse.hpp>
// TerraLib includes
#include <terralib/common/progress/TaskProgress.h>
#include <terralib/core/translator/Translator.h>
#include <terralib/sam/rtree/Index.h>  // R-tree
// STL Includes
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
// GeoDMA Includes
#include "geodmalib/common/messageMediator.hpp"
#include "geodmalib/featureextraction/intersectionCache.hpp"

gdma::fte::IntersectionCache::IntersectionCache(
    const std::vector<te::gm::Geometry*> &layer1,
    const boost::bimap<std::string, std::size_t> &layer1IDs,
    const std::vector<te::gm::Geometry*> &layer2,
    const boost::bimap<std::string, std::size_t> &layer2IDs)
{
    te::common::TaskProgress task(TE_TR("Coumputing intersections"));
    task.setTotalSteps((layer1.size() + layer2.size()));
    task.useTimer(true);

    std::set<std::size_t> invalidGeometriesLayer2;

    for (std::size_t pos = 0; pos < layer2.size(); pos++) {
        if (layer2[pos]) {
            m_rtree.insert(*(layer2[pos]->getMBR()), pos);
        } else {
            std::string msg = "Geometry \"" + layer2IDs.right.find(pos)->second
                + "\" of the Patches layer is null! Operation can not proceed.";
            throw te::common::Exception(TE_TR(msg));
        }

        task.pulse();
    }

    try {
        std::ostringstream output;

        for (std::size_t posL1 = 0; posL1 < layer1.size(); posL1++) {
            te::gm::Geometry* cell = layer1[posL1];
            std::vector<std::size_t> intersections;
            int numIntersections;
            if (cell) {
                numIntersections = m_rtree.search(*(cell->getMBR()), intersections);
            } else {
                std::string msg = "Geometry \"" + layer1IDs.right.find(posL1)->second
                    + "\" of the Landscape layer is null! Operation can not proceed.";
                throw te::common::Exception(TE_TR(msg));
            }


            boost::numeric::ublas::compressed_vector<te::gm::GeometryShrPtr> matrixLine(layer2.size());

            if (numIntersections != 0) {
                for (std::size_t posInt = 0; posInt < numIntersections; posInt++) {
                    std::size_t posL2 = intersections[posInt];
                    te::gm::Geometry* polygon = layer2[posL2];

                    if (polygon->isValid()) {
                        te::gm::GeometryShrPtr intersectionGeom(cell->intersection(polygon));

                        if (!intersectionGeom->isEmpty()) {
                            matrixLine(posL2) = intersectionGeom;
                        }
                    } else {
                        std::string cellId = layer1IDs.right.find(posL1)->second;

                        matrixLine.clear();
                        m_invalidgeometriesLayer1.insert(posL1);
                        invalidGeometriesLayer2.insert(posL2);
                        break;
                    }
                }
            }

            m_intersection_matrix.push_back(matrixLine);

            task.pulse();
        }

        if (m_invalidgeometriesLayer1.size() > 0) {
            if (m_invalidgeometriesLayer1.size() < 20) {
                output << "The following geometries are invalid or have topological problems:"
                    << std::endl;

                for (auto &pos : invalidGeometriesLayer2) {
                    output << "'" << layer2IDs.right.find(pos)->second << "' ";
                }

                output << std::endl << "Operation will not be computed for the folowing ";
                output << "target geometries:" << std::endl;

                for (auto &pos : m_invalidgeometriesLayer1) {
                    output << "'" << layer1IDs.right.find(pos)->second << "' ";
                }
            } else {
                output << "Many geometries are invalid or have topological problems. Operation "
                    << "will not be computed for the corresponding target geometries." << std::endl;
            }

            output << std::endl << std::endl << "No data value will be assumed!" << std::endl;

            gdma::common::MessageMediator::getInstance().sendWarningMsg(output.str());
        }
    } catch (std::exception &e) {
        gdma::common::MessageMediator::getInstance().sendErrorMsg(e.what());
    } catch (te::common::Exception &e) {
        gdma::common::MessageMediator::getInstance().sendErrorMsg(e.what());
    } catch (...) {
        gdma::common::MessageMediator::getInstance().sendErrorMsg("Unknown Error.");
    }
}

gdma::fte::IntersectionCache::~IntersectionCache() {}

bool gdma::fte::IntersectionCache::searchForIntersections(std::size_t geomPositionLayer1,
    MatrixLine& matrixLine) {
    matrixLine = m_intersection_matrix[geomPositionLayer1];
    return (m_invalidgeometriesLayer1.find(geomPositionLayer1) == m_invalidgeometriesLayer1.end());
}

bool gdma::fte::IntersectionCache::isInvalid(std::size_t geomPositionLayer1) {
    return (m_invalidgeometriesLayer1.find(geomPositionLayer1) != m_invalidgeometriesLayer1.end());
}
