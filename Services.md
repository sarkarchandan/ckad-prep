# Services

Services are defined by K8s as an abstract way to expose an application running on a set of Pods as network service. K8s gives Pods their own IP addresses and a single DNS name for a set of Pods and can load balance among them. Needless to say Pods can live or die, means their lifetime can be limited. Hence, trying to reach the application with Pod IP address directly is not a good idea. And their the abstract the unified interface of the K8s Services come in.

We have already discussed the idea of `labels` in our practice examples. Labels are glue, with which a Service can be associated with a set of Pods. The `kube-proxy` of Nodes create virtual IP addresses for Services. Services are not ephemeral / temporary like Pods. Services stick around provides an entry point to the application.

<img src="res/k8s13.png" width="700" height="400" alt="What are Services">

Services create something called endpoint in order to connect with Pods. Following image can intuitively illustrate, what they are. We see in this image, that external callers connects the Service via a externally known endpoint of a Service. Let's say this service encapsulates the Pods, which provides the frontend implementation for some applications and they identify each other using a label e.g., `frontend`. It might be the case, that internally these Pods are then talking to another Service, which creates an internal endpoint for backend service. This internal service and its underlying Pods can then identify themselves with the label e.g., `backend`.

<img src="res/k8s14.png" width="900" height="400" alt="Service orchestration">

[Services](https://kubernetes.io/docs/concepts/services-networking/service/) are one of the most if not the most central idea in K8s. In this note for now we'd stick to the very fundamentals of the Service but there are a lot more to the service, that we eventually need ot understand to be fluent with K8s. There are four main types of Services in K8s.

- `ClusterIP` - Default Service, which exposes the service on a cluster's internal IP. In simple terms we can only access this service from the inside of the cluster. When we run K8s and do a first time `kubectl get all` we see the default ClusterIP service running.

- `NodePort` - Expose a service on the external IP address of a given Node along with a static port on that Node. In the default case NodePort service automatically routes to ClusterIP service.

- `LoadBalancer` - Provision an external IP to act as a load balancer for the service. We have already seen a simpler version of this service in action earlier.

- `ExternalName` - Maps a service to a DNS name or an IP address.

We'd now look at each of the aforementioned category of services in some detail.

## ClusterIP

```bash
$ k get all --show-labels

NAME                 TYPE        CLUSTER-IP   EXTERNAL-IP   PORT(S)   AGE   LABELS
service/kubernetes   ClusterIP   10.96.0.1    <none>        443/TCP   21d   component=apiserver,provider=kubernetes
```

This is the ClusterIP service. This service is exposed internally within the cluster, means the IP above 10.96.0.1 is only accessible from the inside of the cluster, which is also setup in this machine.

<img src="res/k8s15.png" width="900" height="400" alt="ClusterIP">

Only the Pods within the cluster can talk to this service and also among each other using this Service if they need to.

## NodePort

<img src="res/k8s16.png" width="900" height="300" alt="NodePort">

NodePort Service exposes the application service on the external IP address of the Node. In order to do that K8s automatically allocates a static port from the range 30000 - 32767 but it is possible to override this setting. NodePort service creates a way to talk to the Pods running inside a given Node. So in this example an external user can communicate to one of the Pods running inside the specific Node on its external IP address and port 30100. This Service type is often useful for debugging purpose.

## Load Balancer

Load Balancers are important not only to prevent a Node or a Pod to be overburdened with massive amount of incoming traffics but also it is useful for large web-based applications to route the traffic to the Nodes located in different geographic regions. For cloud based applications K8s Load Balancer Service is often useful, when used in combination with the cloud provider's load balancer service. K8s itself has several options, when it comes to implementing a load balancing.

<img src="res/k8s17.png" width="900" height="250" alt="Load Balancer">

In this example we are seeing, two Nodes. We may have NodePort Service setup in these Nodes. Nodes also naturally have their ClusterIP Services running. The external Load Balancer in blue color represents the load balancing service from the cloud provider, which forwards traffics from external callers to one of the Nodes. Nodes may accept the request using Node Port Service before using the internal K8s Load Balancer service or other kinds of Service to route to one of the internal Pods.

## External Name

This is a category of Service, which acts as a name resolution utility for other external Services. For example from the Pods we are running we might have requirement of calling some Service from an external domain. If we configure the IP address / port number etc. for the external service, every time they change we would need to make changes in our Containers as well. External Name Service can abstract away such details and provide a unified interface or a proxy to an external services.

<img src="res/k8s18.png" width="700" height="250" alt="External Name">

In this example we see a simpler scenario for this. The external service details are encapsulated under the External Name Service and instead of talking to the external Services directly our Pods talk to the External Name Service. Now if the details of the external service changes we don't need to make changes in every single running Containers. Instead we would just adapt the External Name Service.

Now, that we have provided some brief introduction to the basic K8s service types we can look at some more granular utilities. Some of these we have already used before.
