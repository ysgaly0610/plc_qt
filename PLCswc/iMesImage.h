//gsoap ns service name: iMesImage
//gsoap ns service namespace: http://localhost/iMesImage.wsdl
//gsoap ns service location: http://localhost
//gsoap ns service executable: iMesImage.cgi
//gsoap ns service encoding: encoded
//gsoap ns schema namespace: urn:iMesImage
int ns__uploadData (char *macCode,char *wipEntityId, char **result);
int ns__DownloadData (char *data,char **value);
int ns__test1(std::string input,std::string &result);


