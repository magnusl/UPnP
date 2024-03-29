/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "service.h"
#include <iostream>
#include "codegen.h"
#include "types.h"

using namespace std;

namespace upnp
{
    /**
     * UPnP_ControlCodegen::GenerateClientDecl
     */
    int UPnP_ControlCodegen::GenerateClientDecl(std::ostream & os)
    {
        os << "#include <control.h>" << endl;
        os << "#include <cstdint>" << endl << endl;
        os << "class "<< m_name << "Client : public upnp::UPnP_ControlPoint" << endl;
        os << "{" << endl;
        os << "public:" << endl;
        GenerateConstructor(os);
        os << endl;
        for(list<UPnP_ServiceAction>::const_iterator it = m_service.m_actions.begin();
            it != m_service.m_actions.end();
            it++)
        {
            GenerateMethodDecl(*it, os);
        }

        os << "};" << endl;
        return 0;
    }

    int UPnP_ControlCodegen::GenerateConstructor(std::ostream & os)
    {
        os << "\t" << m_name << "Client(upnp::UPnP_ClientContext & context, const uuid_t & id, upnp::UPnP_Service & service) : upnp::UPnP_ControlPoint(context, id, service) {}" << endl;
        return 0;
    }

    /**
     * UPnP_ControlCodegen::GenerateMethodDecl
     */
    int UPnP_ControlCodegen::GenerateMethodDecl(const upnp::UPnP_ServiceAction & action, std::ostream & os)
    {
        os << "\t int " << action.m_name;
        GenerateArgumentList(action, os);
        os << ";" << endl;

        return 0;
    }

    /**
     * UPnP_ControlCodegen::GenerateArgumentList
     */
    int UPnP_ControlCodegen::GenerateArgumentList(const upnp::UPnP_ServiceAction & action, std::ostream & os)
    {
        os << "(";
        for(list<UPnP_MethodArgument>::const_iterator it = action.m_arguments.begin();
            it != action.m_arguments.end();
            it++)
        {
            if (it != action.m_arguments.begin())
                os << ", ";
            GenerateArgument(*it, os);
        }
        os << ")";
        return 0;
    }

    /**
     * UPnP_ControlCodegen::GenerateArgument
     */
    int UPnP_ControlCodegen::GenerateArgument(const upnp::UPnP_MethodArgument & argument, std::ostream & os)
    {
        UPnP_StateVariable var;
        /* map the type to the actual implementation type */
        if (!m_service.GetStateVariableByName(argument.m_relatedStateVariable, var))
            return -1;

        const char * nativeType = UPnP_GetImplType(var.m_type);
        if (!nativeType) {
            return -1;
        }

        os << nativeType << ((argument.direction == UPnP_MethodArgument::ARG_DIR_IN) ? " " : " & ") << argument.m_name;
        return 0;
    }

    /**
     * GenerateClientImpl
     */
    int UPnP_ControlCodegen::GenerateClientImpl(std::ostream & os)
    {
        os << "/*" << endl;
        os << " * Client implementation of service '" << m_name << "' generated by UPnPCompiler." << endl;
        os << " */" << endl;
        os << "#include <encode.h>" << endl;
        os << "#include \"" << m_name << "Client.h\"" << endl;

        os << endl << "using namespace upnp;" << endl << endl;
        for(list<UPnP_ServiceAction>::const_iterator it = m_service.m_actions.begin();
            it != m_service.m_actions.end();
            it++)
        {
            os << "int " << m_name << "Client::" << it->m_name;
            GenerateArgumentList(*it, os);
            os << endl;
            os << "{" << endl;
            GenerateMethodBody(*it, os);
            os << "}" << endl;
            os << endl;
        }
        return 0;
    }

    /**
     * UPnP_ControlCodegen::GenerateMethodBody
     */
    int UPnP_ControlCodegen::GenerateMethodBody(const upnp::UPnP_ServiceAction & action, std::ostream & os)
    {
        os << "\tint res;" << endl;
        os << "std::string temp;" << endl;
        os << "\tUPnP_SOAPAction action, response;" << endl;
        os << "\taction.setActionName(\"" << action.m_name << "\");" << endl;
        GenerateArgumentSerialization(action, os);
        os << "\t" << "/* Perform SOAP call */" << endl;
        os << "\t" << "res = PerformSoapAction(action, response);" << endl;
        os << "\t" << "if (res)" << endl;
        os << "\t\t" << "return res;" << endl;
        /* deserialize arguments */
        GenerateResponseDeserialization(action, os);
        os << "\n";
        os << "\treturn res;" << endl;
        return 0;
    }

    int UPnP_ControlCodegen::GenerateArgumentSerialization(const upnp::UPnP_ServiceAction & action, std::ostream & os)
    {
        for(list<UPnP_MethodArgument>::const_iterator it = action.m_arguments.begin();
            it != action.m_arguments.end();
            it++)
        {
            if (it->direction == UPnP_MethodArgument::ARG_DIR_OUT)
                continue;

            UPnP_StateVariable var;
            /* map the type to the actual implementation type */
            if (!m_service.GetStateVariableByName(it->m_relatedStateVariable, var))
                return -1;

            const char * encode = UPnP_GetSerializeMethod(var.m_type);
            os << "\t" << "action.addArgument(\"" << it->m_name <<"\", " << "UPnP_TypeEncode::Encode" << encode << "(" << it->m_name << "));" << endl;
        }

        return 0;
    }

    /**
     *
     */
    int UPnP_ControlCodegen::GenerateResponseDeserialization(const upnp::UPnP_ServiceAction & action, std::ostream & os)
    {
        for(list<UPnP_MethodArgument>::const_iterator it = action.m_arguments.begin();
            it != action.m_arguments.end();
            it++)
        {
            if (it->direction != UPnP_MethodArgument::ARG_DIR_OUT)
                continue;

            UPnP_StateVariable var;
            /* map the type to the actual implementation type */
            if (!m_service.GetStateVariableByName(it->m_relatedStateVariable, var))
                return -1;

            const char * encode = UPnP_GetSerializeMethod(var.m_type);
            os << "\t" << "if (!response.getArgumentValueByName(\"" << it->m_name << "\", temp)) return -1;" << endl;
            os << "\t" << "if (!UPnP_TypeEncode::Decode" << encode << "(" << it->m_name << ", temp)) return -1;" << endl;
        }
        return 0;
    }
}