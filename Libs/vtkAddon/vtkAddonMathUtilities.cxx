/*=auto==============================================================================

  Program: 3D Slicer

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

===============================================================================auto=*/

#include <regex>
#include <sstream>

#include <vtkAddonMathUtilities.h>
#include <vtkMath.h>
#include <vtkMatrix3x3.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtksys/RegularExpression.hxx>
#include <vtkLoggingMacros.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkAddonMathUtilities);

//----------------------------------------------------------------------------
vtkAddonMathUtilities::vtkAddonMathUtilities()
{
}

//----------------------------------------------------------------------------
vtkAddonMathUtilities::~vtkAddonMathUtilities()
{
}

//----------------------------------------------------------------------------
void vtkAddonMathUtilities::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);  
}

//----------------------------------------------------------------------------
bool vtkAddonMathUtilities::MatrixAreEqual(const vtkMatrix4x4* m1,
                                           const vtkMatrix4x4* m2,
                                           double tolerance)
{
  for (int i = 0; i < 4; i++)
    {
    for (int j = 0; j < 4; j++)
      {
      if ( fabs(m1->GetElement(i, j) - m2->GetElement(i, j)) >= tolerance )
        {
        return false;
        }
      }
    }
  return true;
}

//----------------------------------------------------------------------------
bool vtkAddonMathUtilities::MatrixAreEqual(const vtkMatrix4x4 *m1,
                                           const vtkMatrix3x3 *m2,
                                           double tolerance)
{
  for (int i = 0; i < 3; i++)
    {
    for (int j = 0; j < 3; j++)
      {
      if ( fabs(m1->GetElement(i, j) - m2->GetElement(i, j)) >= tolerance )
        {
        return false;
        }
      }
    }
  return true;
}

//----------------------------------------------------------------------------
bool vtkAddonMathUtilities::MatrixAreEqual(const vtkMatrix3x3 *m1,
                                           const vtkMatrix4x4 *m2,
                                           double tolerance)
{
  return MatrixAreEqual(m2, m1, tolerance);
}

//----------------------------------------------------------------------------
bool vtkAddonMathUtilities::MatrixAreEqual(const vtkMatrix3x3 *m1,
                                           const vtkMatrix3x3 *m2,
                                           double tolerance)
{
  for (int i = 0; i < 3; i++)
    {
    for (int j = 0; j < 3; j++)
      {
      if ( fabs(m1->GetElement(i, j) - m2->GetElement(i, j)) >= tolerance )
        {
        return false;
        }
      }
    }
  return true;
}

//----------------------------------------------------------------------------
void vtkAddonMathUtilities::GetOrientationMatrix(vtkMatrix4x4* source,
                                                 vtkMatrix3x3* dest)
{
  if (!source || !dest)
    {
    return;
    }
  for (int ii = 0; ii < 3; ++ii)
    {
    for (int jj = 0; jj < 3; ++jj)
      {
      dest->SetElement(ii, jj, source->GetElement(ii, jj));
      }
    }
}

//----------------------------------------------------------------------------
std::string vtkAddonMathUtilities::ToString(const vtkMatrix4x4* mat, const std::string delimiter, const std::string rowDelimiter)
{
  if (!mat)
    {
    return "";
    }

  std::stringstream ss;
  for (int i = 0; i < 4; i++)
    {
    for (int j = 0; j < 4; j++)
      {
      ss << mat->GetElement(i, j);
      ss << delimiter;
      }
    ss << rowDelimiter;
    }

  return ss.str();
}

//----------------------------------------------------------------------------
bool vtkAddonMathUtilities::FromString(vtkMatrix4x4* mat, const std::string& str, const std::string delimiterExp)
{
  if (!mat)
  {
    return false;
  }

  // Parse the string using the regular expression
  vtksys::RegularExpression delimiterRegex( delimiterExp );

  // Convert each string token into a double and put into vector
  char* end;
  std::string remainString = str;
  std::vector<double> elements;
  while(!remainString.empty())
    {
    bool separatorFound = delimiterRegex.find(remainString);
    std::string::size_type tokenStartIndex = remainString.length();
    std::string::size_type tokenEndIndex = remainString.length();
    if (separatorFound)
      {
      tokenStartIndex = delimiterRegex.start(0);
      tokenEndIndex = delimiterRegex.end(0);
      }
    std::string valString = remainString.substr(0, tokenStartIndex);
    double val = std::strtod(valString.c_str(), &end);
    if (*end != 0)
      {
      return false; // Parsing failed due to non-numeric character
      }
    if (valString.length() > 0) // Ignore if the length is zero (indicates back-to-back delimiters)
      {
      elements.push_back(val);
      }
    remainString = remainString.substr(tokenEndIndex);
    }

  // Ensure the matrix is 1x1, 2x2, 3x3, or 4x4
  if (elements.size() != 1
    && elements.size() != 4
    && elements.size() != 9
    && elements.size() != 16)
    {
    return false;
    }
  int dimension = std::sqrt(elements.size()) + 0.5; // Since conversion to int just truncates

  // Put into matrix
  int linearIndex = 0;
  for (int i = 0; i < dimension; i++)
    {
    for (int j = 0; j < dimension; j++)
      {
      mat->SetElement(i, j, elements.at(linearIndex));
      linearIndex++;
      }
    }

  return true;
}
