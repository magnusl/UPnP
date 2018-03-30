/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "ServiceImpl.h"
#include "arguments.h"

namespace upnp
{
    using namespace std;

    bool generateActionInvocation(const UPnP_ServiceAction & action, std::ostream & os);
    bool generateNativeVariables(const UPnP_ServiceAction & action, std::ostream & os);
    bool deserializeArguments(const UPnP_ServiceAction & action, std::ostream & os);
    bool serializeReturnValues(const UPnP_ServiceAction & action, std::ostream & os);


    /**
     * ServiceImpl::ServiceImpl
     */
    ServiceImpl::ServiceImpl(const std::string & name, const UPnP_Service & service) : m_name(name), m_service(service)
    {
    }

    /**
     * ServiceImpl::~ServiceImpl
     */
    ServiceImpl::~ServiceImpl()
    {
    }

    /*************************************************************************/
    /*                              DECLARATIONS                             */
    /*************************************************************************/

    /**
     * ServiceImpl::generateServiceDecl
     */
    bool ServiceImpl::generateServiceDecl(std::ostream & os) const
    {
        /** class ServiceName_Service : public upnp::UPnP_ServiceImplBase */
        os << "#include <UPnP_ServiceImplBase.h>" << endl;
        os << endl;
        os << "class " << m_name << "_Service : public upnp::UPnP_ServiceImplBase" << endl;
        os << "{" << endl;
        os << "public:" << endl;
        os << "\t" << m_name << "_Service();" << endl;  // constructor
        os << "\t~" << m_name << "_Service();" << endl; // destructor 
        os << "private:" << endl;
        os << "\tvirtual bool InvokeAction(const upnp::UPnP_SOAPAction & action, upnp::UPnP_SOAPAction & response);" << endl;
        os << endl;
        generateActionDecls(os);
        os << endl;
        generateWrapDecls(os);
        os << "};" << endl;
        return true;
    }

    /**
     * ServiceImpl::generateWrapDecls
     */
    bool ServiceImpl::generateWrapDecls(std::ostream & os) const
    {
        os << "\t" << "/* Wrappers */" << endl;
        /** int ActionName_Wrap(const upnp::UPnP_SOAPAction & action, upnp::UPnP_SOAPAction & response) */
        for( list<UPnP_ServiceAction>::const_iterator it = m_service.m_actions.begin(); it != m_service.m_actions.end(); it++)
        {
            os << "\t" << it->m_name << "_Wrap(const upnp::UPnP_SOAPAction & action, upnp::UPnP_SOAPAction & response);" << endl;
        }
        return true;
    }

    /**
     * ServiceImpl::generateActionDecls
     */
    bool ServiceImpl::generateActionDecls(std::ostream & os) const
    {
        os << "\t" << "/* Actions */" << endl;
        for( list<UPnP_ServiceAction>::const_iterator it = m_service.m_actions.begin(); it != m_service.m_actions.end(); it++)
        {
            os << "\t" << "int " << it->m_name;
            upnp::GenerateArgumentList(*it, m_service, os);
            os << ";" << endl;
        }
        return true;
    }

    /*************************************************************************/
    /*                              IMPLEMENTATIONS                          */
    /*************************************************************************/

    bool ServiceImpl::generateWrapImpls(std::ostream & os) const
    {
        return false;
    }

    /**
     * ServiceImpl::generateWrapImpl
     */
    bool ServiceImpl::generateWrapImpl(const UPnP_ServiceAction & action, std::ostream & os) const
    {
        /** const upnp::UPnP_SOAPAction & action, upnp::UPnP_SOAPAction & response */
        os << "int " << action.m_name << "_Wrap(const upnp::UPnP_SOAPAction & action, upnp::UPnP_SOAPAction & response)" << std::endl;
        os << "{" << std::endl;

        generateNativeVariables(action, os);    /** generate the native variable declarations */
        deserializeArguments(action, os);       /** deserialize the arguments from the SOAP request */
        generateActionInvocation(action, os);   /** generate the actual invocation */
        serializeReturnValues(action, os);      /** serialize the return values */

        os << "}" << std::endl;

        return true;
    }

    /**
     * generateActionInvocation
     */
    bool generateActionInvocation(const UPnP_ServiceAction & action, std::ostream & os)
    {
        size_t index = 0, count = action.m_arguments.size();
        os << "int res = " << action.m_name << "(" << std::endl;
        for(list<UPnP_MethodArgument>::const_iterator it = action.m_arguments.begin(); it != action.m_arguments.end(); it++, index++)
        {
            os << "\t\t" << it->m_name << (((index + 1 ) < count) ? "," : "") << endl;
        }
        os << ");" << std::endl;
        return true;
    }
    
    /**
     * generateNativeVariables
     */
    bool generateNativeVariables(const UPnP_ServiceAction & action, std::ostream & os)
    {
        for(list<UPnP_MethodArgument>::const_iterator it = action.m_arguments.begin(); it != action.m_arguments.end(); it++)
        {
        }
        return true;
    }

    /**
     * deserializeArguments
     */
    bool deserializeArguments(const UPnP_ServiceAction & action, std::ostream & os)
    {
        /** Deserialize all the input variables to native variables */
        for(list<UPnP_MethodArgument>::const_iterator it = action.m_arguments.begin(); it != action.m_arguments.end(); it++)
        {
            if (it->direction == UPnP_MethodArgument::ARG_DIR_IN) 
            {
                /** Retrive the named argument and decode it to a native type */
                os << "\t" << "if (!getArgumentValueByName(" << it->m_name << ", tmp) || !UPnP_TypeDecode::Decode(tmp, " 
                    << it->m_name << ")) return -1;" << std::endl;
            }
        }
        return true;
    }

    /**
     * serializeReturnValues
     */
    bool serializeReturnValues(const UPnP_ServiceAction & action, std::ostream & os)
    {
        return false;
    }
}