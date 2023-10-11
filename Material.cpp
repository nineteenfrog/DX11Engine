#include "Material.h"

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader() { return std::shared_ptr<SimpleVertexShader>(); }
std::shared_ptr<SimplePixelShader> Material::GetPixelShader() { return std::shared_ptr<SimplePixelShader>(); }

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vs) { this->vertexShader = vs; }
void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> ps) { this->pixelShader = ps; }
