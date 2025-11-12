import socket

def perform_dns_lookup():
    """
    Performs DNS forward and reverse lookups based on user input.
    """
    while True:
        print("\n--- DNS Lookup Menu ---")
        print("1. Forward Lookup (Domain Name -> IP Address)")
        print("2. Reverse Lookup (IP Address -> Domain Name)")
        print("3. Exit")
        
        choice = input("Enter your choice (1, 2, or 3): ")

        if choice == '1':
            # Option 1: Forward Lookup (Domain -> IP) [cite: 65, 68]
            domain_name = input("Enter the domain name (e.g., www.google.com): ")
            try:
                # Call the resolver function [cite: 70, 71]
                ip_address = socket.gethostbyname(domain_name)
                print(f"\nDomain Name: {domain_name}")
                print(f"IP Address: {ip_address}") # [cite: 73]
            except socket.gaierror:
                # Display error if lookup fails [cite: 84]
                print(f"\nError: Host not found for '{domain_name}'")

        elif choice == '2':
            # Option 2: Reverse Lookup (IP -> Domain) [cite: 65, 75]
            ip_address = input("Enter the IP address (e.g., 142.250.190.36): ")
            try:
                # Call the resolver function for reverse DNS [cite: 81]
                hostname, _, _ = socket.gethostbyaddr(ip_address)
                print(f"\nIP Address: {ip_address}")
                print(f"Host Name: {hostname}") # [cite: 83]
            except socket.herror:
                # Display error if lookup fails [cite: 84]
                print(f"\nError: Host not found for '{ip_address}'")
            except socket.gaierror:
                print(f"\nError: Invalid IP address format for '{ip_address}'")

        elif choice == '3':
            # Step 5: End the program [cite: 85]
            print("Exiting program.")
            break
            
        else:
            print("\nInvalid choice. Please enter 1, 2, or 3.")

if __name__ == "__main__":
    # Step 1: Start the program [cite: 63]
    perform_dns_lookup()
    