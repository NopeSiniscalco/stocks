import wrds

#   crsp_data = db.raw_sql("SELECT date,prc,cusip FROM crsp_q_stock.dsf WHERE cusip = '67066G10'")
#   crsp_header=db.raw_sql("SELECT permno,permco,cusip,htick FROM crsp_q_stock.dsfhdr WHERE htick = 'META'")


# Query the CRSP stock price data for the specified date range and ticker
    #crsp_data = db.raw_sql("SELECT date,prc,cusip FROM crsp_q_stock.dsf WHERE date BETWEEN "+start_date+" AND "+end_date+" AND cusip = "+cusip+"ORDER BY date ASC")

def get_crsp_stock_data(cusip,start_date):
    crsp_data = db.raw_sql("SELECT date,prc,cfacpr FROM crsp_q_stock.dsf WHERE cusip = '"+cusip+"' AND prc > 0 AND date >= '"+start_date+"'ORDER BY date ASC")
    return crsp_data

def get_cusip_from_ticker(ticker):
    crsp_header = db.raw_sql("SELECT cusip,htick FROM crsp_q_stock.dsfhdr WHERE htick = '"+ticker+"'")
    print("Header:")
    print(crsp_header,"\n\n")
    if(crsp_header.shape[0]!=1):
        print("Error in cusip fetch!")
        return -1
    return(crsp_header.iat[0,0])

def interactive_main(save_dir,start_date):
    inp=input("Enter a ticker to get data for: ")
    ticker=inp.upper()
    # Connect to the WRDS database
    cusip=get_cusip_from_ticker(ticker)
    if(cusip==-1):
        quit()
    crsp_data = get_crsp_stock_data(cusip,start_date)
    crsp_data['prc'] = crsp_data['prc']/crsp_data['cfacpr']
    print(crsp_data)
    print("Writing to file...\n")
    crsp_data.to_csv(save_dir+ticker.lower()+"-adjusted.csv",index_label='n',columns=['date','prc'])
    print("Done saving!\n")


# Example usage
#start_date = "'2010-01-01'"
#end_date = "'2020-01-31'"
#cusip = "'67066G10'"

dir="data/"
db = wrds.Connection(wrds_username="siniscalco")
start_date=input("Enter starting date: ")
while(True):
    interactive_main(dir,start_date)
    print("\nRestarting...\n\n")




