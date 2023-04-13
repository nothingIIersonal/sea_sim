#include <sea_sim/interconnect/interconnect.h>

Interconnect::Interconnect(Endpoint module_endpoint) : module_endpoint(module_endpoint) {}

void Interconnect::print_json()
{
    while (const auto &val = module_endpoint.TryRead())
    {
        std::cout << "Received packet:\n";
        std::cout << "\tTo: "    << val.value().to    << "\n";
        std::cout << "\tFrom: "  << val.value().from  << "\n";
        std::cout << "\tEvent: " << val.value().event << "\n";
        std::cout << "\tData: "  << val.value().data  << "\n";
    }
    std::cout << std::endl;
}

void Interconnect::send_json(channel_value_type data)
{
    module_endpoint.SendData(data);
}