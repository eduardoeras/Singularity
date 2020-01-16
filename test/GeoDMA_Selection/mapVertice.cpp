
//   Copyright (C) 2012 National Institute For Space Research (INPE) - Brazil.
//
//   This file is part of the GeoDMA - a Toolbox that integrates Data Mining Techniques with object-based
//   and multi-temporal analysis of satellite remotely sensed imagery.
//
//   GeoDMA is free software: you can redistribute it and/or modify
//   it under the terms of the GNU Lesser General Public License as published by
//   the Free Software Foundation, either version 3 of the License,
//   or (at your option) any later version.
//
//   GeoDMA is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//   GNU Lesser General Public License for more details.
//
//   You should have received a copy of the GNU Lesser General Public License
//   along with GeoDMA. See COPYING. If not, write to
//   GeoDMA Team at <thales@dpi.inpe.br, raian@dpi.inpe.br, castejon@dpi.inpe.br>.


#include <terralib/rp.h>
#include <terralib/classification.h>
#include <terralib/common.h>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <string>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <limits>

// GeoDMA Includes
#include "mapVertice.hpp"
#include "analysis.hpp"
#include "context.hpp"
#include "project.hpp"
#include "ioFunctions.hpp"
#include "geometryFunctions.hpp"
#include "rasterFunctions.hpp"

#define GDMA_SA_VERTICE_TYPE "MAP"

namespace
{
  static gdma::sa::MAPVerticeFactory segmenterVerticeFactoryInstance;
}

namespace gdma {
namespace sa {

  MAPVertice::MAPVertice()
  {
    reset();
  }

  MAPVertice::MAPVertice(const MAPVertice& other)
  {
    operator=( other );
  }

  MAPVertice::~MAPVertice()
  {
  }

  MAPVertice& MAPVertice::operator=( const MAPVertice& other )
  {
    {
      std::lock_guard<std::mutex> lock( getMutex() );
      std::lock_guard<std::mutex> lock2( other.getMutex() );

      reset();

      m_filterIterations = other.m_filterIterations;
      m_inputImageURI = other.m_inputImageURI;
      m_samplesURI = other.m_samplesURI;
      m_samplesClassLablesPropertyName = other.m_samplesClassLablesPropertyName;
    }

    GraphVertice::operator=( other );

    return *this;
  }

  std::string MAPVertice::getType() const
  {
    return std::string( GDMA_SA_VERTICE_TYPE );
  }

  void MAPVertice::getDescription( std::string& descriptionStr ) const
  {
    descriptionStr = "A Maximum a Posteriori classifying vertice cabable of classify the output of a given input vertice or a given input rasters.";
  }

  bool MAPVertice::setParameter( const std::string& parameterName,
    const std::string& parameterValue )
  {
    std::lock_guard<std::mutex> lock( getMutex() );

    if( te::common::Convert2UCase( parameterName ) == "SAMPLES_CLASS_LABELS_PROPERTY_NAME" )
    {
      m_samplesClassLablesPropertyName = parameterValue;
      return true;
    }
    else if( te::common::Convert2UCase( parameterName ) == "FILTER_ITERATIONS" )
    {
      try
      {
        m_filterIterations = boost::lexical_cast< unsigned int >( parameterValue );
      }
      catch(...)
      {
        return false;
      }
      return true;
    }

    return false;
  }

  void MAPVertice::getParameters( std::vector< std::pair< std::string, std::string > >& parameters ) const
  {
    parameters.clear();

    std::lock_guard<std::mutex> lock( getMutex() );

    parameters.push_back( std::pair< std::string, std::string >(
      "FILTER_ITERATIONS", boost::lexical_cast< std::string >(m_filterIterations ) ) );
    parameters.push_back( std::pair< std::string, std::string >(
      "SAMPLES_CLASS_LABELS_PROPERTY_NAME", m_samplesClassLablesPropertyName ) );
  }

  bool MAPVertice::setResource( const std::string& contextURIAlias,
    const std::string& verticeURIAlias )
  {
    if(
        ( getGraph() == 0 )
        ||
        ( getGraph()->getAnalysis() == 0 )
      )
    {
      throw std::runtime_error( "Using an uninitialized context URI Vertice" );
    }

    te::core::URI resorceURI;

    if(
        getGraph()->getAnalysis()->getContext().getResource( contextURIAlias, resorceURI )
        &&
        resorceURI.isValid()
      )
    {
      std::lock_guard<std::mutex> lock( getMutex() );

      if( te::common::Convert2UCase( verticeURIAlias ) == "IMAGE" )
      {
        m_inputImageURI = resorceURI;
        return true;
      }
      else if( te::common::Convert2UCase( verticeURIAlias ) == "SAMPLES" )
      {
        m_samplesURI = resorceURI;
        return true;
      }
    }
    else
    {
      return false;
    }

    return false;
  }

  void MAPVertice::getInParsDescription( std::map< std::string, std::string >& inParsDesc ) const
  {
    inParsDesc.clear();
    inParsDesc[ "SAMPLES_CLASS_LABELS_PROPERTY_NAME" ] = std::string( "Samples class labels property (column) name" );
    inParsDesc[ "FILTER_ITERATIONS" ] = std::string( "Number of mode filter iterations (0 will disable the filter)" );
  }

  void MAPVertice::getInResDescription( std::map< std::string, std::string >& inResDesc ) const
  {
    inResDesc.clear();
    inResDesc[ "IMAGE" ] = std::string( "Input image URI" );
    inResDesc[ "SAMPLES" ] = std::string( "Samples file URI (a semicolon-separated text file)" );
  }

  GraphVertice::OutputType MAPVertice::getOutputType() const
  {
    return GraphVertice::VECTOR_URI_OUT_TYPE;
  }

  GraphVertice* MAPVertice::clone() const
  {
    MAPVertice* vertPtr = new MAPVertice( *this );
    return vertPtr;
  }

  bool MAPVertice::isConnectable( const GraphVertice& other,
    const std::string& alias ) const
  {
    if( other.getOutputType() == VECTOR_URI_OUT_TYPE )
    {
      std::vector< std::pair< std::string, GraphVertice* > > connections;
      getInputConnections( connections );

      if( connections.empty() )
      {
        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }

  bool MAPVertice::isMultiClass() const
  {
    return true;
  }

  bool MAPVertice::is2ndPassCapable() const
  {
    return true;
  }

  void MAPVertice::reset()
  {
    GraphVertice::reset();

    m_filterIterations = 0;
    m_inputImageURI = te::core::URI();
    m_samplesURI = te::core::URI();
    m_samplesClassLablesPropertyName.clear();
  }

  bool MAPVertice::executeFirstPass( std::string& errorMessage )
  {
    if(
        ( ! isCacheEnabled() )
        ||
        ( ! boost::filesystem::exists( get1stPassOutputFileName() ) )
      )
    {
      // check internal parameters

      if( m_samplesClassLablesPropertyName.empty() )
      {
        errorMessage = "Missing samples class labels property name parameter";
        return false;
      }

      // Create the output directory

      if( ! createDirectory( getOutputDirectoryName() ) )
      {
        errorMessage = "Output directory creation error";
        return false;
      }

      // Clear old output files

      {
        const boost::filesystem::path outFilePath( get1stPassOutputFileName() );
        const boost::filesystem::path outFilePathWithoutExtension =
          outFilePath.parent_path() / outFilePath.stem();
        boost::filesystem::directory_iterator dirIt( outFilePath.parent_path() );
        const boost::filesystem::directory_iterator dirItE;

        while( dirIt != dirItE )
        {
          if( boost::filesystem::is_regular_file( *dirIt ) )
          {
            const boost::filesystem::path otherFilePathWithoutExtension =
              dirIt->path().parent_path() / dirIt->path().stem();

            if( outFilePathWithoutExtension == otherFilePathWithoutExtension )
            {
              boost::filesystem::remove( dirIt->path() );
            }
          }

          ++dirIt;
        }
      }

      //  trying to open the input raster

      std::unique_ptr< te::rst::Raster > inputRasterPointer;

      if(
          m_inputImageURI.isValid()
          &&
          ( ! openRaster( m_inputImageURI, inputRasterPointer ) )
        )
      {
        errorMessage = "Raster open error";
        return false;
      }

      // input connection

      std::vector< std::pair< std::string, GraphVertice* > > inputVertConnections;
      getInputConnections( inputVertConnections );

      if( inputVertConnections.size() > 1 )
      {
        errorMessage = "Invalid number of input vertice connections (just 1 allowed)";
        return false;
      }

      // If there is no input data return an error

      if( inputVertConnections.empty() && ( inputRasterPointer.get() == 0 ) )
      {
        errorMessage = "Input image missing or input vertice missing";
        return false;
      }

      // Load the input vertice output result

      std::vector< std::string >  inputExtraFeaturesLabels;
      boost::ptr_vector< te::gm::Geometry > inputGeometries;
      std::vector< std::vector< double > > inputExtraFeatures;

      if( ! inputVertConnections.empty() )
      {
        if( ! loadResultShapeFileExtraPropNames(
          inputVertConnections[ 0 ].second->get1stPassOutputFileName(),
          inputExtraFeaturesLabels ) )
        {
          errorMessage = "Error loading input vertice extra property names";
          return false;
        }

        if( ! loadResultShapeFile(
          inputVertConnections[ 0 ].second->get1stPassOutputFileName(),
          ( inputRasterPointer.get() == 0 ) ? &inputExtraFeaturesLabels : 0,
          &inputGeometries,
          0,
          0,
          ( inputRasterPointer.get() == 0 ) ? &inputExtraFeatures : 0 ) )
        {
          errorMessage = "Error loading input vertice result";
          return false;
        }

        if( inputGeometries.empty() )
        {
           // generate an empty output and returns if no geometries were loaded
          if( ! createEmptyResultShapeFile( get1stPassOutputFileName() ) )
          {
            errorMessage = "1St pass output file creaation error";
            return false;
          }

          return true;
        }
      }

      // Raster clipping / reprojection

      if(
          ( inputRasterPointer.get() != 0 )
          &&
          ( !inputGeometries.empty() )
        )
      {
        te::gm::Envelope inputGeometriesEnv;
        int inputVerticeGeometriesSRID = 0;
        if( ! getGeomSetEnvelope( inputGeometries, inputGeometriesEnv,
          inputVerticeGeometriesSRID ) )
        {
          errorMessage = "Input geometries envelope calcule error";
          return false;
        }

        std::unique_ptr< te::rst::Raster > auxRasterPointer;
        if( ! clipAndReprojectRaster( *inputRasterPointer, &inputGeometriesEnv,
          inputVerticeGeometriesSRID, auxRasterPointer ) )
        {
          errorMessage = "Input raster clipping/reprojection error";
          return false;
        }

        inputRasterPointer.reset( auxRasterPointer.release() );
      }

      // Load samples

      std::vector< std::string > sampleAttributesNames;
      std::vector< std::vector< double > > samples;
      std::vector< std::string > sampleLabels;
      std::size_t sampleLabelsSize = 0;
      unsigned int samplesDimsNmb = 0;

      {
        if( ! m_samplesURI.isValid() )
        {
          errorMessage = "Invalid samples URI";
          return false;
        }

        std::string samplesResourceStr = m_samplesURI.uri();

        if( te::common::Convert2UCase( samplesResourceStr.substr( 0, 7 ) ) != "FILE://" )
        {
          errorMessage = "Invalid samples URI";
          return false;
        }

        samplesResourceStr = samplesResourceStr.substr( 7, samplesResourceStr.size() - 7 );

        if(
            ( ! loadSamplesTextFile( samplesResourceStr, ";", m_samplesClassLablesPropertyName,
              0, sampleAttributesNames, samples, sampleLabels ) )
            ||
            samples.empty()
          )
        {
          errorMessage = "Error loading samples file";
          return false;
        }

        sampleLabelsSize = sampleLabels.size();
        samplesDimsNmb = samples[ 0 ].size();
      }

      // Checking if samples properties match

      if( inputRasterPointer.get() == 0  )
      {
        for( std::size_t sampleAttributesNamesIdx = 0 ; sampleAttributesNamesIdx < sampleAttributesNames.size() ;
          ++sampleAttributesNamesIdx )
        {
          if( std::find( inputExtraFeaturesLabels.begin(), inputExtraFeaturesLabels.end(),
            sampleAttributesNames[ sampleAttributesNamesIdx ] ) == inputExtraFeaturesLabels.end() )
          {
            errorMessage = "Samples property \"" + sampleAttributesNames[ sampleAttributesNamesIdx ]
              + "\" not found in input vertice data properties (";

            for( std::size_t inputExtraFeaturesLabelsIdx = 0 ; inputExtraFeaturesLabelsIdx <
              inputExtraFeaturesLabels.size() ; ++inputExtraFeaturesLabelsIdx )
            {
              if( inputExtraFeaturesLabelsIdx )
              {
                errorMessage += " , ";
              }

              errorMessage += inputExtraFeaturesLabels[ inputExtraFeaturesLabelsIdx ];
            }

            errorMessage += ")";

            return false;
          }
        }
      }
      else
      {
        if( samplesDimsNmb != inputRasterPointer->getNumberOfBands() )
        {
          errorMessage = "Dimensions mistach between the given samples and the input raster bands number";
          return false;
        }
      }

      // Class labels indexes maps

      std::map< std::string, std::size_t > sampleClassLabels2ClassIntLabelMap;
      std::map< std::size_t, std::string > sampleClassIntLabel2ClassLabelMap;

      {
        std::size_t classIdx = 1;

        for( std::size_t sampleLabelsIdx = 0 ; sampleLabelsIdx < sampleLabelsSize ;
          ++sampleLabelsIdx )
        {
          if( sampleClassLabels2ClassIntLabelMap.find( sampleLabels[ sampleLabelsIdx ] ) ==
            sampleClassLabels2ClassIntLabelMap.end() )
          {
            sampleClassLabels2ClassIntLabelMap[ sampleLabels[ sampleLabelsIdx ] ] = classIdx;
            sampleClassIntLabel2ClassLabelMap[ classIdx ] = sampleLabels[ sampleLabelsIdx ];

            ++classIdx;
          }
        }
      }

      // What we want to classify ?

      if( inputRasterPointer.get() == 0 )
      {  // No input raster was given, so the input vertice geoemtries will be classified.
        // training

        te::cl::MAP::Parameters params;

        te::cl::MAP classifierInstance;

        if( ! classifierInstance.initialize( params ) )
        {
          errorMessage = "Classifier initialization error - " + te::rp::Module::getLastLogStr();
          return false;
        }

        te::cl::VectorOfVectorsInputAdaptor< double > samplesInputAdaptor( samples );

        std::vector< unsigned int > samplesAttributeIndexes;
        for( unsigned int sampleAttIdx = 0 ; sampleAttIdx < samplesDimsNmb ;
          ++sampleAttIdx )
        {
          samplesAttributeIndexes.push_back( sampleAttIdx );
        }

        std::vector<unsigned int> integerSamplesLabels;
        for( std::size_t sampleLabelsIdx = 0 ; sampleLabelsIdx < sampleLabelsSize ;
          ++sampleLabelsIdx )
        {
          integerSamplesLabels.push_back( (unsigned int)sampleClassLabels2ClassIntLabelMap[
           sampleLabels[ sampleLabelsIdx ] ] );
        }

        if( ! classifierInstance.train( samplesInputAdaptor, samplesAttributeIndexes,
          integerSamplesLabels, false ) )
        {
          errorMessage = "Classifier trainning error";
          return false;
        }

        // clasifying

        std::vector< unsigned int > selectedInputExtraFeaturesIndexes;

        for( std::size_t sampleAttributesNamesIdx = 0 ; sampleAttributesNamesIdx < sampleAttributesNames.size() ;
          ++sampleAttributesNamesIdx )
        {
          for( std::size_t inputExtraFeaturesLabelsIdx = 0 ; inputExtraFeaturesLabelsIdx <
            inputExtraFeaturesLabels.size() ; ++inputExtraFeaturesLabelsIdx )
          {
            if( sampleAttributesNames[ sampleAttributesNamesIdx ] == inputExtraFeaturesLabels[
              inputExtraFeaturesLabelsIdx ] )
            {
              selectedInputExtraFeaturesIndexes.push_back( inputExtraFeaturesLabelsIdx );
              break;
            }
          }
        }

        te::cl::VectorOfVectorsInputAdaptor< double > inputExtraFeaturesInputAdaptor( inputExtraFeatures );

        std::vector<unsigned int> classifierOutput( inputGeometries.size() );
        te::cl::VectorOutputAdaptor< unsigned int > classifiedLabelsOutputAdaptor( classifierOutput );

        std::vector< double > inputNoDataValues( selectedInputExtraFeaturesIndexes.size(),
          std::numeric_limits< double >::max() );

        if( ! classifierInstance.classify( inputExtraFeaturesInputAdaptor, selectedInputExtraFeaturesIndexes,
          inputNoDataValues, classifiedLabelsOutputAdaptor, 0, 0.0, false ) )
        {
          errorMessage = "Classifier running error";
          return false;
        }

        // Preparing output

        std::vector< std::string > outputLabels;
        std::vector< GraphVerticeIDT > outputIDs;

        {
          const std::size_t classifierOutputSize = classifierOutput.size();

          outputLabels.resize( classifierOutputSize );
          outputIDs.resize( classifierOutputSize );

          for( std::size_t classifierOutputIdx = 0 ; classifierOutputIdx <
            classifierOutputSize ; ++classifierOutputIdx )
          {
            outputLabels[ classifierOutputIdx ] = sampleClassIntLabel2ClassLabelMap[
              classifierOutput[ classifierOutputIdx ] ];
            outputIDs[ classifierOutputIdx ] = (GraphVerticeIDT)classifierOutput[
              classifierOutputIdx ];
          }
        }

        // Export to the output file

        if( ! save2ResultShapeFile( inputGeometries, get1stPassOutputFileName(),
          &outputLabels, &outputIDs, 0, 0 ) )
        {
          errorMessage = "Output file creation error";
          return false;
        }
      }
      else
      {  // A input raster was given, so it will be classified
        std::unique_ptr< te::rst::Raster > classifiedRasterPtr;

        {
          // samples convertion

          te::rp::ClassifierMAPStrategy::Parameters::MClassesSamplesCTPtr allClassesSamplesPtr(
            new te::rp::ClassifierMAPStrategy::Parameters::MClassesSamplesCT());

          const std::size_t samplesSize = samples.size();

          for( std::size_t samplesIdx = 0 ; samplesIdx < samplesSize ; ++samplesIdx )
          {
            allClassesSamplesPtr->operator[]( sampleClassLabels2ClassIntLabelMap[ sampleLabels[
              samplesIdx ] ] ).push_back( samples[ samplesIdx ] );
          }

          /* Define classification parameters */
          /* Input parameters */

          te::rp::Classifier::InputParameters classAlgoInputParameters;

          classAlgoInputParameters.m_inputRasterPtr = inputRasterPointer.get();

          for( unsigned int bandIdx = 0 ; bandIdx < inputRasterPointer->getNumberOfBands() ;
            ++bandIdx )
          {
            classAlgoInputParameters.m_inputRasterBands.push_back( bandIdx );
          }

          /* Link specific parameters with chosen implementation */

          te::rp::ClassifierMAPStrategy::Parameters classifierparameters;
          classifierparameters.m_trainSamplesPtr = allClassesSamplesPtr;
          classifierparameters.m_prioriCalcSampleStep = 5;

          classAlgoInputParameters.m_strategyName = "map";
          classAlgoInputParameters.setClassifierStrategyParams(classifierparameters);

          /* Output parameters */

          te::rp::Classifier::OutputParameters classAlgoOutputParameters;
          classAlgoOutputParameters.m_rType = "EXPANSIBLE";

          /* Execute the algorithm */

          te::rp::Classifier classAlgorithmInstance;

          if( ! classAlgorithmInstance.initialize( classAlgoInputParameters ) )
          {
            errorMessage = "MAP algorithm initialization error - " + te::rp::Module::getLastLogStr();
            return false;
          }

          if( ! classAlgorithmInstance.execute( classAlgoOutputParameters ) )
          {
            errorMessage = "MAP algorithm execution error - "  + te::rp::Module::getLastLogStr();
            return false;
          }

          classifiedRasterPtr.reset( classAlgoOutputParameters.m_outputRasterPtr.release() );

//          te::rp::Copy2DiskRaster( *classifiedRasterPtr, "classified.tif" );
        }

        // Median filter

        if( m_filterIterations )
        {
          /* Creating the algorithm parameters */

          te::rp::Filter::InputParameters filterAlgoInputParams;
          filterAlgoInputParams.m_filterType = te::rp::Filter::InputParameters::ModeFilterT;
          filterAlgoInputParams.m_inRasterPtr = classifiedRasterPtr.get();
          filterAlgoInputParams.m_inRasterBands.push_back( 0 );
          filterAlgoInputParams.m_iterationsNumber = m_filterIterations;
          filterAlgoInputParams.m_windowH = filterAlgoInputParams.m_windowW = 3;

          te::rp::Filter::OutputParameters filterAlgoOutputParams;
          filterAlgoOutputParams.m_rType = "EXPANSIBLE";

          /* Executing the algorithm */

          te::rp::Filter filterAlgorithmInstance;

          if( ! filterAlgorithmInstance.initialize( filterAlgoInputParams ) )
          {
            errorMessage = "Filter algorithm initialization error - " + te::rp::Module::getLastLogStr();
            return false;
          }

          if( ! filterAlgorithmInstance.execute( filterAlgoOutputParams ) )
          {
            errorMessage = "Filter algorithm execution error - "  + te::rp::Module::getLastLogStr();
            return false;
          }

          classifiedRasterPtr.reset( filterAlgoOutputParams.m_outputRasterPtr.release() );

//          te::rp::Copy2DiskRaster( *classifiedRasterPtr, "filtered.tif" );
        }

        // Vectorization

        boost::ptr_vector< te::gm::Geometry > classifiedVectors;
        std::vector< GraphVerticeIDT > classifiedVectorsIDs;

        if( ! vectorizeResultRaster( *classifiedRasterPtr, classifiedVectors,
          &classifiedVectorsIDs ) )
        {
          errorMessage = "Vectorization error";
          return false;
        }

        // Free anused resources

        classifiedRasterPtr.reset();
        inputRasterPointer.reset();

        // Intersection and export

        if( inputGeometries.empty() )
        {
          // Export to the output file

          if( ! save2ResultShapeFile( classifiedVectors, get1stPassOutputFileName(),
            0, &classifiedVectorsIDs, 0, 0 ) )
          {
            errorMessage = "Output file creation error";
            return false;
          }
        }
        else
        {
          // Intersetion of vectorization result with the input vertex 1st pass output

          boost::ptr_vector< te::gm::Geometry > intersectionResult;
          std::vector< std::size_t > intersectionOriginalIndexes;
          geometrySetsIntersection( classifiedVectors, inputGeometries,
             intersectionResult, &intersectionOriginalIndexes );

          // Preparing output

          std::vector< GraphVerticeIDT > outputIDs;

          {
            const std::size_t intersectionOriginalIndexesSize = intersectionOriginalIndexes.size();

            outputIDs.resize( intersectionOriginalIndexesSize );

            for( std::size_t intersectionOriginalIndexesIdx = 0 ; intersectionOriginalIndexesIdx <
              intersectionOriginalIndexesSize ; ++intersectionOriginalIndexesIdx )
            {
              outputIDs[ intersectionOriginalIndexesIdx ] = classifiedVectorsIDs[
                intersectionOriginalIndexes[ intersectionOriginalIndexesIdx ] ];
            }
          }

          // Export to the output file

          if( ! save2ResultShapeFile( intersectionResult, get1stPassOutputFileName(),
            0, &outputIDs, 0, 0 ) )
          {
            errorMessage = "Output file creation error";
            return false;
          }
        }
      }
    }

    return true;
  }

  //  -------------------------------------------------------------------------

  MAPVerticeFactory::MAPVerticeFactory()
  : GraphVerticeFactory( GDMA_SA_VERTICE_TYPE )
  {}

  MAPVerticeFactory::~MAPVerticeFactory()
  {}

  GraphVertice* MAPVerticeFactory::build()
  {
    return new MAPVertice();
  }

}  // end namespace sa
}  // end namespace gdma
