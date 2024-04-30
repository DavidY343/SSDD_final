import zeep

def main():
    wsdl_url = "http://localhost:8000/?wsdl"
    soap = zeep.Client(wsdl=wsdl_url) 
    result = soap.service.get_time()
    print(result)

if __name__ == '__main__':
    main()

